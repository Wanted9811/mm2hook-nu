#include "mmHudMapFeatureHandler.h"

using namespace MM2;

/*
    mmHudMapFeatureHandler
*/

int hudMapColorStyle = 0;
int playerTriColor = 5;
int playerTriOutlineColor = 0;
int policeTriColor = 1;
int policeTriOutlineColor = 0;
int opponentTriColor = 7;
int opponentTriOutlineColor = 0;

static ConfigValue<int> cfgHudMapColorStyle("HudMapColorStyle", 0);
static ConfigValue<int> cfgPlayerTriColor("PlayerTriColor", 5);
static ConfigValue<int> cfgPlayerTriOutlineColor("PlayerTriOutlineColor", 0);
static ConfigValue<int> cfgPoliceTriColor("PoliceTriColor", 1);
static ConfigValue<int> cfgPoliceTriOutlineColor("PoliceTriOutlineColor", 0);
static ConfigValue<int> cfgOpponentTriColor("OpponentTriColor", 7);
static ConfigValue<int> cfgOpponentTriOutlineColor("OpponentTriOutlineColor", 0);

unsigned int HudmapIconColors[14] = {
    0xFF000000, // Black
    0xFFFF0000, // Red
    0xFF0000EF, // Blue
    0xFF00EF00, // Green
    0xFFEF0000, // Dark Red
    0xFFFFFF00, // Yellow
    0xFFFF5A00, // Orange
    0xFFB400FF, // Purple
    0xFF00FFFF, // Aqua
    0xFFFF0390, // Pink

    /* NEW ADDITIONAL COLORS */

    0xFFFFFFFF, // White
    0xFF3F3F3F, // Grey
    0xFFFDBF72, // Light Orange
    0xFFC0EC42, // Light Green
};

void mmHudMapFeatureHandler::DrawColoredTri(unsigned int color, const Matrix34& matrix)
{
    rglEnableDisable(RGL_DEPTH_TEST, false);
    gfxRenderState::SetWorldMatrix(matrix);
    vglBindTexture(0);
    vglBegin(DRAWMODE_TRIANGLELIST, 0);
    vglCurrentColor = color;
    vglVertex(0.f, 0.f, -1.f);
    vglVertex(-0.69999999f, 0.f, 1.f);
    vglVertex(0.69999999f, 0.f, 1.f);
    vglEnd();
    rglEnableDisable(RGL_DEPTH_TEST, true);
}

void mmHudMapFeatureHandler::DrawIcon(int iconColor, const Matrix34& matrix)
{
    auto map = reinterpret_cast<mmHudMap*>(this);

    Matrix34 mtx = Matrix34();
    mtx.Set(matrix);

    mtx.SetRow(1, Vector3::YAXIS);
    mtx.Normalize();

    mtx.m31 += 15.f;
    mtx.Scale(map->GetIconScale());

    DrawColoredTri(HudmapIconColors[iconColor], mtx);
}

void mmHudMapFeatureHandler::DrawOutlinedIcon(int iconColor, int iconOutlineColor, const Matrix34& matrix)
{
    Matrix34 tempMatrix;
    tempMatrix.Set(matrix);

    auto map = reinterpret_cast<mmHudMap*>(this);
    float iconScaleOld = map->GetIconScale();
    map->SetIconScale(iconScaleOld * 1.3f);

    DrawIcon(iconOutlineColor, tempMatrix);

    map->SetIconScale(iconScaleOld);

    tempMatrix.m31 += 1.1f;
    DrawIcon(iconColor, tempMatrix);
}

void mmHudMapFeatureHandler::DrawOutlinedIcon(int iconColor, const Matrix34& matrix)
{
    DrawOutlinedIcon(iconColor, 0, matrix);
}

void mmHudMapFeatureHandler::DrawPlayer()
{
    auto map = reinterpret_cast<mmHudMap*>(this);
    auto player = map->GetPlayer();
    auto playerMtx = *map->GetPlayerMatrix();
    auto car = player->GetCar();
    auto audio = car->GetCarAudioContainerPtr();
    auto siren = car->GetSiren();
    char *vehName = car->GetCarDamage()->GetName();

    float time = 0.1f;
    bool elapsedTime1 = fmod(datTimeManager::ElapsedTime, time * 0.75f) > time * 0.25f;
    bool elapsedTime2 = fmod(datTimeManager::ElapsedTime, time * 0.75f) > time * 0.5f;
    bool elapsedTime3 = fmod(datTimeManager::ElapsedTime, 0.5f) > 0.25f;

    if (hudMapColorStyle == 0) // MM2
    {
        DrawOutlinedIcon(5, playerMtx);
    }

    if (hudMapColorStyle == 1) // MM1
    {
        if (audio->IsPolice(vehName)) 
        {
            DrawOutlinedIcon(1, 2, playerMtx);
        }
        else 
        {
            DrawOutlinedIcon(0, 10, playerMtx);
        }
    }

    if (hudMapColorStyle == 2) // NFSHP2
    {
        int iconColor = 5;
        if (audio->IsPolice(vehName))
        {
            iconColor = 2;
            if (siren != nullptr && siren->IsActive())
            {
                if (elapsedTime3)
                    iconColor = 1;
            }
        }
        DrawOutlinedIcon(iconColor, playerMtx);
    }

    if (hudMapColorStyle == 3) // NFSMW
    {
        int iconColor = 12;
        if (audio->IsPolice(vehName)) 
        {
            iconColor = 10;
            if (siren != nullptr && siren->IsActive()) 
            {
                if (elapsedTime1)
                    iconColor = 1;
                if (elapsedTime2)
                    iconColor = 2;
            }
        }
        DrawOutlinedIcon(iconColor, playerMtx);
    }

    if (hudMapColorStyle == 4) // NFSC
    {
        int iconColor = 8;
        if (audio->IsPolice(vehName))
        {
            iconColor = 4;
            if (siren != nullptr && siren->IsActive())
            {
                iconColor = 10;
                if (elapsedTime1)
                    iconColor = 1;
                if (elapsedTime2)
                    iconColor = 2;
            }
        }
        DrawOutlinedIcon(iconColor, playerMtx);
    }

    if (hudMapColorStyle >= 5) // CUSTOM
    {
        DrawOutlinedIcon(playerTriColor, playerTriOutlineColor, playerMtx);
    }

    // Hook to allow for custom icon drawing
    MM2Lua::OnRenderHudmap();
}

void mmHudMapFeatureHandler::DrawCops()
{
    auto map = reinterpret_cast<mmHudMap*>(this);
    auto AIMAP = aiMap::GetInstance();

    float time = 0.1f;
    bool elapsedTime1 = fmod(datTimeManager::ElapsedTime, time * 0.75f) > time * 0.25f;
    bool elapsedTime2 = fmod(datTimeManager::ElapsedTime, time * 0.75f) > time * 0.5f;
    bool elapsedTime3 = fmod(datTimeManager::ElapsedTime, 0.5f) > 0.25f;
    bool showAllCops = map->GetShowAllCops();

    for (int i = 0; i < AIMAP->numCops; i++)
    {
        auto police = AIMAP->Police(i);
        auto policeMtx = police->GetCar()->GetICS()->GetMatrix();
        WORD policeState = police->GetPoliceState();

        // check if the cop in pursuit
        if (policeState || showAllCops)
        {
            if (hudMapColorStyle == 0) // MM2
            {
                DrawOutlinedIcon(1, policeMtx);
            }

            if (hudMapColorStyle == 1) // MM1
            {
                DrawOutlinedIcon(1, 2, policeMtx);
            }

            if (hudMapColorStyle == 2) // NFSHP2
            {
                int iconColor = 2;
                if (elapsedTime3)
                    iconColor = 1;
                if (policeState == 12 || policeState == 0)
                    iconColor = 2;
                DrawOutlinedIcon(iconColor, policeMtx);
            }

            if (hudMapColorStyle == 3) // NFSMW
            {
                int iconColor = 10;
                if (elapsedTime1)
                    iconColor = 1;
                if (elapsedTime2)
                    iconColor = 2;
                if (policeState == 12 || policeState == 0)
                    iconColor = 10;
                DrawOutlinedIcon(iconColor, policeMtx);
            }

            if (hudMapColorStyle == 4) // NFSC
            {
                int iconColor = 10;
                if (elapsedTime1)
                    iconColor = 1;
                if (elapsedTime2)
                    iconColor = 2;
                if (policeState == 12 || policeState == 0)
                    iconColor = 4;
                DrawOutlinedIcon(iconColor, policeMtx);
            }

            if (hudMapColorStyle >= 5) // CUSTOM
            {
                DrawOutlinedIcon(policeTriColor, policeTriOutlineColor, policeMtx);
            }
        }
    }
}

void mmHudMapFeatureHandler::DrawOpponents()
{
    auto AIMAP = aiMap::GetInstance();
    auto map = reinterpret_cast<mmHudMap*>(this);

    for (int i = 0; i < map->GetOpponentCount(); i++)
    {
        auto iconInfo = map->GetOpponentIcon(i);
        if (iconInfo->Enabled && iconInfo->Matrix != nullptr)
        {
            auto opponentMtx = *iconInfo->Matrix;

            // check if we're in multiplayer
            if (MMSTATE->unk_EC)
            {
                DrawOutlinedIcon(i + 2, opponentMtx);
            }
            else
            {
                auto opponent = AIMAP->Opponent(i);
                auto car = opponent->GetCar();
                auto curDamage = car->GetCarDamage()->GetCurDamage();
                auto maxDamage = car->GetCarDamage()->GetMaxDamage();

                if (curDamage < maxDamage)
                {
                    switch (hudMapColorStyle)
                    {
                        case 0: // MM2
                        {
                            DrawOutlinedIcon(7, opponentMtx);
                            break;
                        }
                        case 1: // MM1
                        {
                            DrawOutlinedIcon(i + 2, opponentMtx);
                            break;
                        }
                        case 2: // NFSHP2
                        {
                            DrawOutlinedIcon(3, opponentMtx);
                            break;
                        }
                        case 3: // NFSMW
                        {
                            DrawOutlinedIcon(13, opponentMtx);
                            break;
                        }
                        case 4: // NFSC
                        {
                            DrawOutlinedIcon(6, opponentMtx);
                            break; 
                        }
                        case 5: // CUSTOM
                        {
                            DrawOutlinedIcon(opponentTriColor, opponentTriOutlineColor, opponentMtx);
                            break;
                        }
                        default:
                        {
                            DrawOutlinedIcon(7, opponentMtx);
                            break;
                        }
                    }
                }
                else 
                {
                    DrawOutlinedIcon(11, opponentMtx);
                }
            }
        }
    }
}

void mmHudMapFeatureHandler::Install() {
    hudMapColorStyle = cfgHudMapColorStyle.Get();
    playerTriColor = cfgPlayerTriColor.Get();
    playerTriOutlineColor = cfgPlayerTriOutlineColor.Get();
    policeTriColor = cfgPoliceTriColor.Get();
    policeTriOutlineColor = cfgPoliceTriOutlineColor.Get();
    opponentTriColor = cfgOpponentTriColor.Get();
    opponentTriOutlineColor = cfgOpponentTriOutlineColor.Get();

    InstallCallback("mmHudMap::DrawPlayer",
        &DrawPlayer, {
            cb::call(0x42F527),
        }
    );

    InstallCallback("mmHudMap::DrawCops",
        &DrawCops, {
            cb::call(0x42F519),
        }
    );

    InstallCallback("mmHudMap::DrawOpponents",
        &DrawOpponents, {
            cb::call(0x42F520),
        }
    );

    InstallCallback("mmHudMap::DrawIcon", "Draw outline on any icon drawn via DrawIcon",
        static_cast<void(mmHudMapFeatureHandler::*)(int, const Matrix34&)>(&DrawOutlinedIcon), {
            cb::jmp(0x42F580),
        }
    );
}

