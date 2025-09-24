#include "cityLevelHandler.h"
#include "sdlPage16Handler.h"
#include <handlers\lua_drawable.h>
#include "vglHandler.h"

using namespace MM2;

// ==========================
// Pointer hooks
// ==========================

static float ped_LodThreshold = 1225.f;

hook::Type<float> obj_NoDrawThresh(0x5C571C); // default: 300.0

hook::Type<float> obj_VLowThresh(0x5C6658); // default: 200.0
hook::Type<float> obj_LowThresh(0x5C665C); // default: 100.0
hook::Type<float> obj_MedThresh(0x5C6660); // default: 40.0

hook::Type<float> sdl_VLowThresh(0x5C5708);  // default: 300.0
hook::Type<float> sdl_LowThresh(0x5C570C);  // default: 100.0
hook::Type<float> sdl_MedThresh(0x5C5710);  // default: 50.0

hook::Type<bool> sdl_SolidColor(0x62B08E);

static hook::Type<bool> sm_PerRoomLighting = 0x5C5720;
static hook::Type<bool> sm_LightInstances = 0x5C5721;
static hook::Type<bool> sm_EnableShadows = 0x5C664C;
static hook::Type<Matrix44> ShadowMatrix = 0x6A3B48;

/*
    cityLevelHandler
*/

int cityLevelHandler::city_numRooms = 0;
int cityLevelHandler::city_currentRoom = 0;

const double cosNum = 1.570796;

/* PSDL shading fix */

static void setLightDirectionInv(Vector3* out, float heading, float pitch) {
    out->X = -(cosf(heading) * cosf(pitch));
    out->Y = -(sinf(pitch));
    out->Z = -(sinf(heading) * cosf(pitch));
}

void cityLevelHandler::UpdateLighting()
{
    // Originally the game calls UpdateLighting() here without first setting up the lights
    reinterpret_cast<cityLevel*>(lvlLevel::GetSingleton())->SetupLighting(Vector3(1.0, 1.0, 1.0));
    sdlCommon::UpdateLighting();

    // Calculate SDL lighting color
    // TODO: fix lighting quality not being taken into account (harder than it sounds)
    auto timeWeather = cityLevel::GetCurrentLighting();
    Vector3 keyDir, fill1Dir, fill2Dir;
    Vector3 lighting = Vector3(0.0f, 0.0f, 0.0f);

    Vector4 ambient;
    ambient.UnpackColorBGRA(timeWeather->Ambient);
    lighting += static_cast<Vector3>(ambient);

    setLightDirectionInv(&keyDir, timeWeather->KeyHeading, timeWeather->KeyPitch);
    setLightDirectionInv(&fill1Dir, timeWeather->Fill1Heading, timeWeather->Fill1Pitch);
    setLightDirectionInv(&fill2Dir, timeWeather->Fill2Heading, timeWeather->Fill2Pitch);

    Vector3 normal = Vector3::YAXIS;
    lighting += timeWeather->KeyColor * fmaxf(0.0f, normal.Dot(keyDir));
    lighting += timeWeather->Fill1Color * fmaxf(0.0f, normal.Dot(fill1Dir));
    lighting += timeWeather->Fill2Color * fmaxf(0.0f, normal.Dot(fill2Dir));

    lighting.X = fminf(1.0f, fmaxf(0.0f, lighting.X));
    lighting.Y = fminf(1.0f, fmaxf(0.0f, lighting.Y));
    lighting.Z = fminf(1.0f, fmaxf(0.0f, lighting.Z));
    
    vglHandler::vglResultColor.r = byte(lighting.X * 255.0f);
    vglHandler::vglResultColor.g = byte(lighting.Y * 255.0f);
    vglHandler::vglResultColor.b = byte(lighting.Z * 255.0f);
    vglHandler::vglResultColor.a = 255;
}

void cityLevelHandler::LightmapDelete(void* data)
{
    // Hooks the lmap data delete
    hook::StaticThunk<0x577380>::Call<void>(data);

    // Set perRoomLighting to TRUE when this happens as the LMAP exists
    sm_PerRoomLighting.set(true);
}

void cityLevelHandler::DrawSDL(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        auto sky = &level->Sky;
        sky->SetupFog(viewport.GetFar());

        gfxRenderState::SetTouchedMask(false);
        gfxRenderState::SetLighting(false);

        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);

        rglWorldIdentity();
        vglBeginBatch();

        auto recs = roomRecs;
        int rooms = numRooms;

        while (rooms)
        {
            int lod = 3;
            float distance = recs->Distance;

            if (distance <= sdl_VLowThresh)
            {
                if (distance <= sdl_LowThresh)
                {
                    if (distance > sdl_MedThresh)
                    {
                        lod = 2;
                    }
                }
                else
                {
                    lod = 1;
                }
            }
            else
            {
                lod = 0;
            }

            int color = level->GetRoomInfo(recs->RoomId)->LightmapColor;
            gfxTexture::sm_LOD = 3 - lod;
            vglCurrentColor = color;
            if (level->IsRoomVisible(recs->RoomId))
                reinterpret_cast<sdlPage16Handler*>(cityLevel::GetSDL()->GetPage(recs->RoomId))->Draw(lod, color);

            ++recs;
            --rooms;
        }

        vglEndBatch();
        gfxTexture::sm_LOD = 0;

        if (sm_LightInstances.get())
        {
            gfxRenderState::SetTouchedMask(true);
            cityLevel::SetupLighting(cityLevel::GetSmLighting());
            gfxRenderState::SetLighting(true);
        }
        else
        {
            gfxRenderState::SetTouchedMask(false);
            cityLevel::SetupLighting(cityLevel::GetSmLighting());
            gfxRenderState::SetLighting(false);
        }

        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawStatics(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);

        int hwRoomCount, swRoomCount;

        if (useSoftware)
        {
            hwRoomCount = 0;
            swRoomCount = numRooms;
        }
        else
        {
            swRoomCount = -1;
            hwRoomCount = numRooms - 1;
        }

        int hwRoom = hwRoomCount;

        int i = useSoftware ? 1 : -1;

        if (hwRoomCount != swRoomCount)
        {
            auto recs = &roomRecs[hwRoomCount];
            int rooms = numRooms;
            while (hwRoom != swRoomCount)
            {
                if (sm_PerRoomLighting.get())
                    level->SetupPerRoomLighting(recs->RoomId);

                for (lvlInstance* j = level->GetRoomInfo(recs->RoomId)->FirstStaticInstance; j; j = j->GetNext())
                {
                    int lod = j->IsVisible(viewport);
                    if (lod)
                    {
                        j->Draw(lod - 1);
                    }
                }
                recs += i;
                hwRoom += i;
            }
        }

        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawRoadDecals(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        if (sm_EnableShadows.get())
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);

            if (!useSoftware)
            {
                auto curViewport = gfxPipeline::GetCurrentViewport();
                auto d3dViewport = curViewport->GetD3DViewport();
                auto dwHeight = d3dViewport.dwHeight;
                auto dwWidth = d3dViewport.dwWidth;
                auto fudge = 1.0f - cityLevel::GetShadowHFudge();
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    dwWidth,
                    dwHeight,
                    fudge,
                    cityLevel::GetShadowHFudge());

                gfxRenderState::SetAlphaEnabled(true);
                gfxRenderState::SetWorldMatrix(ShadowMatrix.get());
                gfxRenderState::SetZWriteEnabled(false);
                gfxRenderState::SetZEnabled(D3DZB_TRUE);

                vglBeginBatch();
                auto recs = roomRecs;
                int rooms = numRooms;
                while (rooms)
                {
                    if (recs->Distance < obj_LowThresh)
                    {
                        auto roomInfo = level->GetRoomInfo(recs->RoomId);
                        if ((roomInfo->InstanceFlags & lvlInstance::INST_SHADOW_RGL) != 0)
                        {
                            for (lvlInstance* j = roomInfo->FirstInstance; j; j = j->GetNext())
                            {
                                if ((j->GetFlags() & lvlInstance::INST_SHADOW_RGL) != 0)
                                {
                                    int lod = j->IsVisible(viewport);
                                    if (lod)
                                    {
                                        j->DrawShadow();
                                    }
                                }
                            }
                        }
                    }
                    ++recs;
                    --rooms;
                }
                vglEndBatch();

                gfxRenderState::SetZWriteEnabled(true);
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    d3dViewport.dwWidth,
                    d3dViewport.dwHeight,
                    0.0f,
                    1.0f);
            }

            if (sm_LightInstances.get())
            {
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
            }
            else
            {
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
            }
        }
    }
}

void cityLevelHandler::DrawStaticShadows(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        if (sm_EnableShadows.get())
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);

            if (!useSoftware)
            {
                auto curViewport = gfxPipeline::GetCurrentViewport();
                auto d3dViewport = curViewport->GetD3DViewport();
                auto dwHeight = d3dViewport.dwHeight;
                auto dwWidth = d3dViewport.dwWidth;
                auto fudge = 1.0f - cityLevel::GetShadowHFudge();
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    dwWidth,
                    dwHeight,
                    fudge,
                    cityLevel::GetShadowHFudge());

                gfxRenderState::SetAlphaEnabled(true);
                gfxRenderState::SetWorldMatrix(ShadowMatrix.get());
                gfxRenderState::SetZWriteEnabled(false);
                gfxRenderState::SetZEnabled(D3DZB_TRUE);

                auto recs = roomRecs;
                int rooms = numRooms;
                while (rooms)
                {
                    if (recs->Distance < obj_LowThresh)
                    {
                        auto roomInfo = level->GetRoomInfo(recs->RoomId);
                        if ((roomInfo->InstanceFlags & lvlInstance::INST_SHADOW) != 0)
                        {
                            for (lvlInstance* j = roomInfo->FirstStaticInstance; j; j = j->GetNext())
                            {
                                if ((j->GetFlags() & lvlInstance::INST_SHADOW) != 0)
                                {
                                    int lod = j->IsVisible(viewport);
                                    if (lod)
                                    {
                                        j->DrawShadow();
                                    }
                                }
                            }
                        }
                    }
                    ++recs;
                    --rooms;
                }

                gfxRenderState::SetZWriteEnabled(true);
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    d3dViewport.dwWidth,
                    d3dViewport.dwHeight,
                    0.0f,
                    1.0f);
            }

            if (sm_LightInstances.get())
            {
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
            }
            else
            {
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
            }
        }
    }
}

void cityLevelHandler::DrawPropShadows(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        if (sm_EnableShadows.get())
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);

            if (!useSoftware)
            {
                auto curViewport = gfxPipeline::GetCurrentViewport();
                auto d3dViewport = curViewport->GetD3DViewport();
                auto dwHeight = d3dViewport.dwHeight;
                auto dwWidth = d3dViewport.dwWidth;
                auto fudge = 1.0f - cityLevel::GetShadowHFudge();
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    dwWidth,
                    dwHeight,
                    fudge,
                    cityLevel::GetShadowHFudge());

                gfxRenderState::SetAlphaEnabled(true);
                gfxRenderState::SetWorldMatrix(ShadowMatrix.get());
                gfxRenderState::SetZWriteEnabled(false);
                gfxRenderState::SetZEnabled(D3DZB_TRUE);

                auto recs = roomRecs;
                int rooms = numRooms;
                while (rooms)
                {
                    if (recs->Distance < obj_LowThresh)
                    {
                        auto roomInfo = level->GetRoomInfo(recs->RoomId);
                        if ((roomInfo->InstanceFlags & lvlInstance::INST_SHADOW) != 0 &&
                            (roomInfo->InstanceFlags & lvlInstance::INST_BANGER) != 0)
                        {
                            for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                            {
                                if ((j->GetFlags() & lvlInstance::INST_SHADOW) != 0 &&
                                    (j->GetFlags() & lvlInstance::INST_BANGER) != 0)
                                {
                                    int lod = j->IsVisible(viewport);
                                    if (lod)
                                    {
                                        j->DrawShadow();
                                    }
                                }
                            }
                        }
                    }
                    ++recs;
                    --rooms;
                }

                gfxRenderState::SetZWriteEnabled(true);
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    d3dViewport.dwWidth,
                    d3dViewport.dwHeight,
                    0.0f,
                    1.0f);
            }

            if (sm_LightInstances.get())
            {
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
            }
            else
            {
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
            }
        }
    }
}

void cityLevelHandler::DrawProps(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        dgTreeRenderer::dgTreeRenderer();

        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);

        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            if (sm_PerRoomLighting.get())
                level->SetupPerRoomLighting(recs->RoomId);

            float distance = recs->Distance;
            if (distance <= obj_NoDrawThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_BANGER) != 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if (lod)
                        {
                            j->Draw(lod - 1);
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }

        gfxRenderState::SetTouchedMask(false);
        gfxRenderState::SetLighting(false);
        dgTreeRenderer::GetInstance()->RenderTrees();

        if (sm_LightInstances.get())
        {
            gfxRenderState::SetTouchedMask(true);
            gfxRenderState::SetLighting(true);
        }
        else
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);
        }

        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawMoverShadows(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        if (sm_EnableShadows.get())
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);

            if (!useSoftware)
            {
                auto curViewport = gfxPipeline::GetCurrentViewport();
                auto d3dViewport = curViewport->GetD3DViewport();
                auto dwHeight = d3dViewport.dwHeight;
                auto dwWidth = d3dViewport.dwWidth;
                auto fudge = 1.0f - cityLevel::GetShadowHFudge();
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    dwWidth,
                    dwHeight,
                    fudge,
                    cityLevel::GetShadowHFudge());

                gfxRenderState::SetAlphaEnabled(true);
                gfxRenderState::SetWorldMatrix(ShadowMatrix.get());
                reinterpret_cast<luaDrawableHandler*>(level)->CallCallbacks(0);
                gfxRenderState::SetZWriteEnabled(false);
                gfxRenderState::SetZEnabled(D3DZB_TRUE);

                auto recs = roomRecs;
                int rooms = numRooms;
                while (rooms)
                {
                    if (recs->Distance < obj_LowThresh)
                    {
                        auto roomInfo = level->GetRoomInfo(recs->RoomId);
                        for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                        {
                            if ((j->GetFlags() & lvlInstance::INST_VISIBLE) != 0 && (j->GetFlags() & lvlInstance::INST_BANGER) == 0)
                            {
                                int lod = j->IsVisible(viewport);
                                if (lod)
                                {
                                    j->DrawShadow();
                                }
                            }
                        }
                    }
                    ++recs;
                    --rooms;
                }

                gfxRenderState::SetZWriteEnabled(true);
                curViewport->SetWindow(
                    d3dViewport.dwX,
                    d3dViewport.dwY,
                    d3dViewport.dwWidth,
                    d3dViewport.dwHeight,
                    0.0f,
                    1.0f);
            }

            if (sm_LightInstances.get())
            {
                gfxRenderState::SetTouchedMask(true);
                gfxRenderState::SetLighting(true);
            }
            else
            {
                gfxRenderState::SetTouchedMask(false);
                gfxRenderState::SetLighting(false);
            }
        }
    }
}

void cityLevelHandler::DrawMovers(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);
        
        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            if (sm_PerRoomLighting.get())
                level->SetupPerRoomLighting(recs->RoomId);

            float distance = recs->Distance;
            if (distance <= obj_NoDrawThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_VISIBLE) != 0 && (j->GetFlags() & lvlInstance::INST_BANGER) == 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if (lod)
                        {
                            j->Draw(lod - 1);
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }
        
        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawAlpha(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);

        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            if (sm_PerRoomLighting.get())
                level->SetupPerRoomLighting(recs->RoomId);

            float distance = recs->Distance;
            if (distance <= obj_NoDrawThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_VISIBLE) != 0 && (j->GetFlags() & lvlInstance::INST_BANGER) == 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if (lod)
                        {
                            j->DrawReflectedParts(lod - 1);
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }

        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawLights(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        gfxTexture::sm_LOD = 0;
        gfxRenderState::SetClipping(true);
        gfxRenderState::SetTouchedMask(false);
        gfxRenderState::SetLighting(false);
        gfxRenderState::SetZWriteEnabled(false);
        gfxRenderState::SetFogEnabled(false);
        gfxRenderState::SetBlendSet(7, 0x80);

        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            float distance = recs->Distance;
            if (distance <= obj_NoDrawThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_GLOW) != 0 || (j->GetFlags() & lvlInstance::INST_SHADOW) != 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if (lod)
                        {
                            j->DrawGlow();
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }
    }
}

void cityLevelHandler::DrawReflected(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            if (sm_PerRoomLighting.get())
                level->SetupPerRoomLighting(recs->RoomId);

            float distance = recs->Distance;
            if (distance <= obj_MedThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_VISIBLE) != 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if ((lod - 1) == 3)
                        {
                            j->DrawReflected(1.0f);
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }
    }
}

void cityLevelHandler::DrawParticles()
{
    rglWorldIdentity();
    gfxRenderState::SetBlendSet(0, 0x80);

    auto level = reinterpret_cast<cityLevel*>(this);
    reinterpret_cast<luaDrawableHandler*>(level)->CallCallbacks(1);

    if (cityLevel::GetRainParticles())
    {
        auto flags = level->GetRoomInfo(level->FindRoomId(static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3)), 0))->Flags;
        if ((flags & 0xA) == 0)
        {
            lvlSegment segment = lvlSegment();
            lvlIntersection intersection = lvlIntersection();
            Vector3 camPosition = Vector3(gfxRenderState::GetCameraMatrix().GetRow(3));
            Vector3 camPos = Vector3(camPosition.X, camPosition.Y + 100.f, camPosition.Z);
            segment.Set(camPos, camPosition, 0, nullptr);

            if ((flags & lvlInstance::INST_WHEELCOLLISION) == 0 || !dgPhysManager::Instance.get()->Collide(segment, &intersection, 0, nullptr, lvlInstance::INST_WHEELCOLLISION, 0))
            {
                camPos.X = 0.0f;
                camPos.Y = 10.0f;
                camPos.Z = -10.0f;
                Vector3 vec = Vector3();
                Matrix34 camMatrix = Matrix34();
                gfxRenderState::GetCameraMatrix().ToMatrix34(camMatrix);
                vec.Dot(camPos, camMatrix);
                cityLevel::GetRainBirthRule()->SetPosition(vec);
                cityLevel::GetRainParticles()->Cull();
            }
        }
    }
}

void cityLevelHandler::DrawStaticGlass(const gfxViewport& viewport, const cityRoomRec* roomRecs, int numRooms)
{
    auto level = reinterpret_cast<cityLevel*>(this);

    if (numRooms)
    {
        byte alphaRef = gfxRenderState::GetAlphaRef();
        byte alphaFunc = gfxRenderState::GetAlphaFunc();

        gfxRenderState::SetAlphaRef(cityLevel::GetRefAlpha());
        gfxRenderState::SetAlphaFunc(D3DCMP_GREATER);

        gfxRenderState::SetFogEnabled(!useSoftware);
        if (sm_LightInstances.get())
        {
            gfxRenderState::SetTouchedMask(true);
            gfxRenderState::SetLighting(true);
        }
        else
        {
            gfxRenderState::SetTouchedMask(false);
            gfxRenderState::SetLighting(false);
        }

        int hwRoomCount, swRoomCount;

        if (useSoftware)
        {
            hwRoomCount = 0;
            swRoomCount = numRooms;
        }
        else
        {
            swRoomCount = -1;
            hwRoomCount = numRooms - 1;
        }

        int hwRoom = hwRoomCount;

        int i = useSoftware ? 1 : -1;

        if (hwRoomCount != swRoomCount)
        {
            auto recs = &roomRecs[hwRoomCount];
            int rooms = numRooms;
            while (hwRoom != swRoomCount)
            {
                if (sm_PerRoomLighting.get())
                    level->SetupPerRoomLighting(recs->RoomId);

                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstStaticInstance; j; j = j->GetNext())
                {
                    int lod = j->IsVisible(viewport);
                    if (lod)
                    {
                        j->DrawReflectedParts(lod - 1);
                    }
                }
                recs += i;
                hwRoom += i;
            }
        }

        auto recs = roomRecs;
        int rooms = numRooms;
        while (rooms)
        {
            if (sm_PerRoomLighting.get())
                level->SetupPerRoomLighting(recs->RoomId);

            float distance = recs->Distance;
            if (distance <= obj_NoDrawThresh)
            {
                auto roomInfo = level->GetRoomInfo(recs->RoomId);
                for (lvlInstance* j = roomInfo->FirstInstance; j != roomInfo->FirstStaticInstance; j = j->GetNext())
                {
                    if ((j->GetFlags() & lvlInstance::INST_BANGER) != 0)
                    {
                        int lod = j->IsVisible(viewport);
                        if (lod)
                        {
                            j->DrawReflectedParts(lod - 1);
                        }
                    }
                }
            }
            ++recs;
            --rooms;
        }

        gfxRenderState::SetAlphaFunc((D3DCMPFUNC)alphaFunc);
        gfxRenderState::SetAlphaRef(alphaRef);
    }
}

void cityLevelHandler::DrawRooms(const gfxViewport& viewport, unsigned int p2, const cityRoomRec* roomRecs, int numRooms)
{
    cityLevelHandler::city_numRooms = numRooms;
    cityLevelHandler::city_currentRoom = 0;
    UpdateLighting();

    ++lvlMultiRoomInstance::sm_Sample;
    gfxRenderState::SetFogEnabled(!useSoftware);
    gfxRenderState::SetFogVertexMode(D3DFOG_LINEAR);

    DrawSDL(viewport, roomRecs, numRooms);
    DrawStatics(viewport, roomRecs, numRooms);
    DrawRoadDecals(viewport, roomRecs, numRooms);
    DrawPropShadows(viewport, roomRecs, numRooms);
    DrawProps(viewport, roomRecs, numRooms);
    DrawMoverShadows(viewport, roomRecs, numRooms);
    DrawMovers(viewport, roomRecs, numRooms);
    DrawAlpha(viewport, roomRecs, numRooms);
    DrawLights(viewport, roomRecs, numRooms);
    DrawReflected(viewport, roomRecs, numRooms);
    DrawParticles();
    DrawStaticGlass(viewport, roomRecs, numRooms);

    gfxRenderState::SetTouchedMask(false);
    gfxRenderState::SetLighting(false);
    gfxRenderState::SetFogEnabled(false);
    gfxRenderState::SetAlphaEnabled(false);
    gfxRenderState::SetZWriteEnabled(true);
    gfxTexture::sm_LOD = 0;
}

void cityLevelHandler::SetObjectDetail(int lod) {
    // Default MM2 values (leaving this here for reference)
    // static float lodLevels[4][4] = {
    //     { 200.0f, 150.0f,  70.0f, 20.0f }, // Low
    //     { 250.0f, 175.0f,  90.0f, 30.0f }, // Medium
    //     { 300.0f, 200.0f, 100.0f, 40.0f }, // High
    //     { 300.0f, 200.0f, 130.0f, 70.0f }, // Very high
    // };

    static const char *lodLevelNames[4] = {
        "Low",
        "Medium",
        "High",
        "Very High",
    };

    // 'Low' now uses 'Medium' values, 'Medium' uses 'High', etc.
    // 'Very high' now renders a bit farther than before, but not to an extreme.
    // Performance drops are to be expected until bugs can be ironed out.
    // Poor PVS optimization seems to be the reason why values were so low.
    static float lodLevels[4][4] = {
        { 250.0f, 175.0f,  90.0f, 30.0f }, // Low       (Default: Medium)
        { 300.0f, 200.0f, 100.0f, 40.0f }, // Medium    (Default: High)
        { 300.0f, 200.0f, 130.0f, 70.0f }, // High      (Default: Very High)
        { 500.0f, 300.0f, 150.0f, 100.0f }, // Very high (NEW)
    };

    // Using temporary variables so we don't need to constantly access pointers

    float objNoDrawThresh = lodLevels[lod][0]; // VL: <VLowThresh> - <NoDrawThresh>
    float objVLowThresh = lodLevels[lod][1]; // L: <LowThresh> - <VLowThresh>
    float objLowThresh = lodLevels[lod][2]; // M: <MedThresh> - <LowThresh>
    float objMedThresh = lodLevels[lod][3]; // H: 0.0 - <MedThresh>

    obj_NoDrawThresh = objNoDrawThresh;
    obj_VLowThresh = objVLowThresh;
    obj_LowThresh = objLowThresh;
    obj_MedThresh = objMedThresh;

    // By default, the game doesn't set these values based on the detail level
    // They are hardcoded to what is considered 'High' by default,
    // however this is now known as 'Medium' (lod = 1; see above)
    //
    // 'Medium' and below (default 'High') uses the defaults.
    float sdlVLowThresh = (lod > 1) ? (objVLowThresh + 100.0f) : 300.0f;
    float sdlLowThresh = (lod > 1) ? (objLowThresh + 25.0f) : 100.0f;
    float sdlMedThresh = (lod > 1) ? (objMedThresh + 10.0f) : 50.0f;

    sdl_VLowThresh = sdlVLowThresh;
    sdl_LowThresh = sdlLowThresh;
    sdl_MedThresh = sdlMedThresh;

    // By default the game doesn't set this based on the detail level
    float pedLodBase = 35.5f;
    float pedDrawThreshold = powf(pedLodBase, 2) * (lod + 1);
    ped_LodThreshold = pedDrawThreshold;

    // Also not set by default
    sdl_SolidColor = (lod <= 1);

    LogFile::Format("[cityLevel::SetObjectDetail]: '%s'\n"
        " - OBJ { %.4f, %.4f, %.4f, %.4f }\n"
        " - SDL { %.4f, %.4f, %.4f }\n"
        " - PED { %.4f }\n",
        lodLevelNames[lod],
        objNoDrawThresh, objVLowThresh, objLowThresh, objMedThresh,
        sdlVLowThresh, sdlLowThresh, sdlMedThresh,
        pedDrawThreshold);
}

MM2::gfxTexture* cityLevelHandler::GetEnvMap(int roomNumber, Vector3 const& position, float& intensity)
{
    auto level = reinterpret_cast<cityLevel*>(this);
    auto room = level->GetRoomInfo(roomNumber);

    if (room->Flags & 2)
    {
        // UNDERGROUND
        intensity = 0.0f;
        return nullptr; 
    }
    else 
    {
        return level->cityLevel::GetEnvMap(roomNumber, position, intensity);
    }
}

void cityLevelHandler::Install() {
    InstallVTableHook("cityLevel::SetObjectDetail", &SetObjectDetail, {
        0x5B16E0
    });

    InstallVTableHook("cityLevel::GetEnvMap", &GetEnvMap, {
        0x5B16F8
    });

    InstallCallback("cityLevel::DrawRooms", "Custom implementation to allow for getting the number of rooms.",
        &DrawRooms, {
            cb::call(0x445798), // cityLevel::Draw
        }
    );

    InstallCallback("cityLevel::Load", "Fix lighting being delayed by one load.",
        &UpdateLighting, {
            cb::call(0x444260),
        }
    );

    InstallCallback("cityLevel::Load - operator delete", "Fix PerRoomLighting never getting reset to true",
        &LightmapDelete, {
            cb::call(0x4447B8),
        }
    );

    // moves ped lod threshold to writable memory
    mem::write(0x54BC3D + 2, &ped_LodThreshold);
    mem::write(0x57B6CE + 2, &ped_LodThreshold);

    // move shadow render threshold from furthest possible -> L lod
    mem::write(0x445E23 + 2, obj_LowThresh.ptr());
    mem::write(0x445DC6 + 2, obj_LowThresh.ptr());
}

