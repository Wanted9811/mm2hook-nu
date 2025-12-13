#include "mm2_rgl.h"
#include <modules\city\citylevel.h>
#include <modules\mmcityinfo\state.h>

using namespace MM2;

declfield(MM2::vglIndices)          (0x5C9EBC);
declfield(MM2::vglCloudMapEnable)   (0x661928);
declfield(MM2::vglOffset)           (0x661930);
declfield(MM2::vglVertex)           (0x661970);
declfield(MM2::vglTextures)         (0x6619C4);
declfield(MM2::vglPrimitives)       (0x661A44);
declfield(MM2::vglCurrentColor)     (0x661974);
declfield(MM2::vglVertexSize)       (0x661978);
declfield(MM2::vglVertexBuffer)     (0x661B44);
declfield(MM2::vglVertexFVF)        (0x681B44);
declfield(MM2::vglPrimitiveFlags)   (0x681B48);
declfield(MM2::vglVCT1)             (0x682B48);
declfield(MM2::vglCurrentNZ)        (0x682B4C);
declfield(MM2::vglCurrentNX)        (0x682B50);
declfield(MM2::vglCurrentNY)        (0x682B54);
declfield(MM2::vglCount)            (0x682B58);
declfield(MM2::vglCurrentT)         (0x682B5C);
declfield(MM2::vglCurrentS)         (0x682B60);
declfield(MM2::vglUseCloudMap)      (0x682B64);
declfield(MM2::vglVNT1)             (0x682B68);
declfield(MM2::vglTextureCount)     (0x682B88);
declfield(MM2::vglCloudMapTexture)  (0x682B74);

declfield(MM2::mtxstack)            (0x682F98);

/* lua helper */
void MM2::luaSetColor(const Vector4 & color) {
    vglCurrentColor.set(color.PackColorBGRA());
}

void MM2::luaGfxDrawFont(double x, double y, const char* text) {
    //letting lua handle this gives us 'number has no integer representation' so this is what we do instead
    gfxDrawFont((int)round(x), (int)round(y), text);
}

void MM2::vglUnbindTexture()
{
    vglBindTexture(nullptr);
}

void MM2::vglCreateVertex(float curS, float curT, const Vector3 &vertexPosition)
{
    vglCurrentS = curS;
    vglCurrentT = curT;
    vglVCT1->position = vertexPosition;
    vglVCT1->reserved = 0;
    vglVCT1->color = vglCurrentColor;
    vglVCT1->specular = 0;
    vglVCT1->texCoords = Vector2(vglCurrentS, vglCurrentT);
    ++vglCount;
    ++vglVCT1;
}

/* psdl.obj */

void    MM2::vglTexCoord2f          (Vector2 uv)                                                { vglTexCoord2f(uv.X, uv.Y); }
void    MM2::vglTexCoord2f          (float u, float v)                                          { return _StaticThunk<0x45CFD0>::Call<void>(u, v); }
void    MM2::vglVertex3f            (float x, float y, float z)                                 { return _StaticThunk<0x45CFF0>::Call<void>(x, y, z); }
void    MM2::vglVertex3f            (const Vector3 position)                                    { return _StaticThunk<0x45D080>::Call<void>(position); }

/* vgl.obj */

void    MM2::vgl_VERTEX_VCT1        (float x, float y, float z)                                 { return _StaticThunk<0x4A5370>::Call<void>(x, y, z); }
void    MM2::vgl_VERTEX_VNT1        (float x, float y, float z)                                 { return _StaticThunk<0x4A5400>::Call<void>(x, y, z); }
void    MM2::vglSetFormat           (uint p1, uint p2, uint p3, uint p4)                        { return _StaticThunk<0x4A5490>::Call<void>(p1, p2, p3, p4); }
void    MM2::vglBegin               (gfxDrawMode drawMode, int p2)                              { return _StaticThunk<0x4A5500>::Call<void>(drawMode, p2); }
void    MM2::vglDrawPrimitive       (unsigned int index)                                        { return _StaticThunk<0x4A5910>::Call<void>(index); }
void    MM2::vglEnd                 (void)                                                      { return _StaticThunk<0x4A5A90>::Call<void>(); }
void    MM2::vglBeginBatch          (void)                                                      { return _StaticThunk<0x4A5B10>::Call<void>(); }
void    MM2::vglEndBatch            (void)                                                      { return _StaticThunk<0x4A5B80>::Call<void>(); }
void    MM2::vglBindTexture         (gfxTexture *texture)                                       { return _StaticThunk<0x4A5BA0>::Call<void>(texture); }
void    MM2::vglDrawLabel           (const Vector3 &position, const char *text)                 { return _StaticThunk<0x4A5C50>::Call<void>(&position, text); }

void    MM2::vglDrawTexture         (unsigned int index)
{
    gfxTexture* texture = vglTextures[index];
    texture != nullptr ? texture->BindIndex = 0 : vglTextureCount = 0;

    bool drawCloudMap = false;
    if (vglCloudMapEnable.get() && vglCloudMapTexture.get())
    {
        gfxTexture* texture = vglTextures[index];
        if (texture != nullptr)
        {
            if ((vglCloudMapEnable.get() & texture->TexEnv) != 0)
                drawCloudMap = true;
        }
    }

    gfxRenderState::SetTexture(0, vglTextures[index]);
    gfxRenderState::FlushMasked();
    vglDrawPrimitive(index);

    if (drawCloudMap)
    {
        if (vglUseCloudMap.get())
        {
            bool alphaEnable = gfxRenderState::SetAlphaEnabled(true);
            byte alphaRef = gfxRenderState::SetAlphaRef(0);
            gfxRenderState::SetTexture(0, vglCloudMapTexture);
            gfxRenderState::FlushMasked();
            vglDrawEnvMap(index);
            gfxRenderState::SetAlphaRef(alphaRef);
            gfxRenderState::SetAlphaEnabled(alphaEnable);
        }
        gfxRenderState::FlushMasked();
    }
}

void    MM2::vglDrawEnvMap          (unsigned int index)
{
    int primitive = vglPrimitives[index];
    float scale = vglOffset->m33;
    Vector3 offset = Vector3(vglOffset->m30, vglOffset->m31, vglOffset->m32);

    while (primitive)
    {
        uint flag = vglPrimitiveFlags[primitive];
        int numVerts = (flag >> 0xA) & 0xFFF;
        int adjunctCount = (flag >> 3) & 0x7F;
        int primitiveType = flag & 7;
        primitive = flag >> 22;

        auto vertexBuffer = &vglVertexBuffer[numVerts * vglVertexSize];
        auto verts = reinterpret_cast<vglVNT1_t*>(vertexBuffer);
        auto orthoVerts = reinterpret_cast<vglVCT1_t*>(vertexBuffer);

        for (int i = 0; i < adjunctCount; i++)
        {
            auto& vert = verts[i];
            auto& orthoVert = orthoVerts[i];

            Vector3 movement = cityLevel::EnableMovingClouds ? Vector3::AXIS : Vector3::ORIGIN;
            Vector3 offsetVert = offset + vert.position + (movement * datTimeManager::ElapsedTime);

            orthoVert.texCoords.X = ((offsetVert.X + offsetVert.Y) * scale);
            orthoVert.texCoords.Y = ((offsetVert.Z + offsetVert.Y) * scale);
        }

        lpD3DDev->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primitiveType,
            vglVertexFVF,
            (LPVOID)vertexBuffer,
            adjunctCount,
            (LPWORD)&vglIndices,
            adjunctCount,
            0);
    } 
}

/* font.obj */
void    MM2::gfxDrawFont            (int x, int y, const char *text)                            { return _StaticThunk<0x4B1280>::Call<void>(x, y, text); }

void MM2::vglDrawLabelf(const Vector3 &position, const char *format, ...) {
        va_list va;
        va_start(va, format);
        char buffer[1024];
        vsprintf_s(buffer, format, va);
        va_end(va);

        vglDrawLabel(position, buffer);
}

void    MM2::vglSetCloudMap         (const char *texture)                                       { return _StaticThunk<0x4A5D30>::Call<void>(texture); }
void    MM2::vglSetOffset           (float x, float y, float z)                                 { return _StaticThunk<0x4A5DE0>::Call<void>(x, y, z); }
bool    MM2::rglEnableDisable       (rglToken token, bool value)                                { return _StaticThunk<0x4A5F00>::Call<bool>(token, value); }
bool    MM2::rglIsEnabled           (rglToken token)                                            { return _StaticThunk<0x4A6000>::Call<bool>(token); }
void    MM2::rglWorldMatrix         (const Matrix34 &mtx)                                       { return _StaticThunk<0x4A6060>::Call<void>(&mtx); }
void    MM2::rglWorldIdentity       (void)                                                      { return _StaticThunk<0x4A6110>::Call<void>();  }
void    MM2::rglCard                (const Vector3 &position)                                   { return _StaticThunk<0x4A6140>::Call<void>(&position); }
void    MM2::rglBindTexture2        (const gfxTexture *texture)                                 { return _StaticThunk<0x4A6160>::Call<void>(texture); }
void    MM2::tglDrawParticle        (const Vector3& position, float size, const Vector4& color) { return _StaticThunk<0x4A6190>::Call<void>(&position, size, &color); }
void    MM2::tglDrawRotatedParticle (const Vector3 &p1, float p2, float p3, const Vector4 &p4)  { return _StaticThunk<0x4A6550>::Call<void>(&p1, p2, p3, &p4); }
uint    MM2::mkfrgba                (float r, float g, float b, float a)                        { return _StaticThunk<0x4A7880>::Call<uint>(r, g, b, a); }

void MM2::tglParticleClipAdjust(const Vector3& position, Vector3& outAdjustedPos)
{
    // move towards camera by anticlip
    Vector3 camPosAAA = static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3));
    outAdjustedPos = position;
    Vector3 positionDifference = (position - camPosAAA);
    positionDifference.Scale(positionDifference.InvMag());
    outAdjustedPos.Subtract(positionDifference);
}

void MM2::tglDrawParticleClipAdjusted(const Vector3& position, float size, const Vector4& color)
{
    Vector3 particlePosAAA;
    tglParticleClipAdjust(position, particlePosAAA);
    tglDrawParticle(particlePosAAA, size, color);
}

void MM2::tglDrawCustomShadowedParticle(const Matrix34& matrix, const Vector3& position, dgBangerGlowData* glowData, lvlInstance* instance)
{
    Matrix34 shadowMatrix = Matrix34();
    shadowMatrix.Set(matrix);

    int index = glowData->FlashPerIndex ? glowData->Index : 0;
    int invFlashTime = glowData->InvFlashTime;

    float heightDifference = (position.Y - shadowMatrix.m31);
    float size = heightDifference * glowData->ShadowSize;
    float intensity = fminf(fmaxf(heightDifference * 0.08f, 0.0f), 0.8f);
    Vector4 color = glowData->Color * glowData->Color.W * glowData->ShadowIntensity * (1.0f - intensity);

    if (glowData->FlatMode)
    {
        Vector3 angle = Vector3(glowData->FlatRotation * 0.017453f); // Degrees to Radians
        Vector3 rotationSpeed = Vector3(glowData->RotationSpeed * datTimeManager::ElapsedTime);
        shadowMatrix.Rotate(shadowMatrix.GetRow(1), angle.Y + rotationSpeed.Y);
        shadowMatrix.SetRow(3, shadowMatrix.GetRow(3) + (shadowMatrix.GetRow(2) * size));
    }

    Vector2 flashFreq1 = glowData->FlashFreq;
    bool flashingTime1 = !flashFreq1.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq1.X) > flashFreq1.Y);
    if ((flashingTime1 && !invFlashTime) || (!flashingTime1 && invFlashTime))
    {
        Vector2 flashFreq2 = glowData->FlashFreq2;
        bool flashingTime2 = !flashFreq2.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq2.X) > flashFreq2.Y);
        if (flashingTime2)
            tglDrawShadowedParticle(shadowMatrix, size, color);
    }
}

void MM2::tglDrawShadowedParticle(const Matrix34& shadowMatrix, float size, const Vector4& color)
{
    //get world matrix
    Matrix34 worldMatrix = Matrix34();
    gfxRenderState::GetWorldMatrix().ToMatrix34(worldMatrix);
    worldMatrix.RotateTo(shadowMatrix.GetRow(1), worldMatrix.GetRow(2));

    //get sizes
    float negativeSize = -size;
    float doubleSize = size + size;

    //prepare vertex positions
    Vector3 position = static_cast<Vector3>(worldMatrix.GetColumn(0)) * negativeSize + shadowMatrix.GetRow(3);
    Vector3 vertice1 = static_cast<Vector3>(worldMatrix.GetColumn(1)) * negativeSize + position;
    Vector3 vertice2 = static_cast<Vector3>(worldMatrix.GetColumn(0)) * doubleSize + vertice1;
    Vector3 vertice3 = static_cast<Vector3>(worldMatrix.GetColumn(1)) * doubleSize + vertice2;
    Vector3 vertice4 = static_cast<Vector3>(worldMatrix.GetColumn(1)) * doubleSize + vertice1;

    //setup draw mode, set color and draw particle
    vglBegin(gfxDrawMode::DRAWMODE_TRIANGLEFAN, 4);
    vglCurrentColor = color.PackColorBGRA();
    vglCreateVertex(0.0f, 0.0f, vertice1);
    vglCreateVertex(1.0f, 0.0f, vertice2);
    vglCreateVertex(1.0f, 1.0f, vertice3);
    vglCreateVertex(0.0f, 1.0f, vertice4);
    vglEnd();
}

void MM2::tglDrawCustomProjection(const Matrix34& modelMatrix, dgBangerGlowData* glowData, lvlInstance* instance)
{
    strstr(glowData->Name, "yel") ? vglBindTexture(glowData->YelLightBeam) : vglBindTexture(glowData->LightBeam);

    int index = glowData->FlashPerIndex ? glowData->Index : 0;
    int flatMode = glowData->FlatMode;
    int invFlashTime = glowData->InvFlashTime;

    Vector2 flashFreq = glowData->FlashFreq;
    bool flashTime = !flashFreq.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq.X) > flashFreq.Y);
    if ((flashTime && !invFlashTime) || (!flashTime && invFlashTime))
    {
        Vector2 flashFreq2 = glowData->FlashFreq2;
        bool flashTime2 = !flashFreq2.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq2.X) > flashFreq2.Y);
        if (flashTime2)
        {
            tglDrawProjectionParticle(modelMatrix, glowData);
        }
    }
}

void MM2::tglDrawProjectionParticle(const Matrix34& modelMatrix, dgBangerGlowData* glowData)
{
    //make projection particle double sided
    auto previousCullMode = gfxRenderState::GetCullMode();
    gfxRenderState::SetCullMode(D3DCULL_NONE);
    rglWorldIdentity();
    vglBeginBatch();

    strstr(glowData->Name, "yel") ? vglBindTexture(glowData->YelLightBeam) : vglBindTexture(glowData->LightBeam);

    Vector3 lightPosition = modelMatrix.Transform(glowData->Offset);
    Vector3 particlePosAAA;
    tglParticleClipAdjust(lightPosition, particlePosAAA);

    float sizeX = 6.0f;
    float sizeY = 4.0f;

    Vector3 drawPosition = Vector3(particlePosAAA.X, particlePosAAA.Y - (sizeY - 0.2f), particlePosAAA.Z);
    float intensity = MMSTATE->WeatherType == 2 ? 0.5f : 0.25f;
    Vector4 color = glowData->Color * glowData->Color.W * intensity;

    //get model view matrix
    Matrix34 modelViewMatrix = Matrix34();
    gfxRenderState::GetModelViewMatrix().ToMatrix34(modelViewMatrix);

    //face the camera vertically
    modelViewMatrix.m01 = 0.0f;
    modelViewMatrix.m11 = 1.0f;
    modelViewMatrix.m21 = 0.0f;

    //prepare vertex positions
    Vector3 position = static_cast<Vector3>(modelViewMatrix.GetColumn(0)) * sizeX * -1.0f + drawPosition;
    Vector3 vertice1 = static_cast<Vector3>(modelViewMatrix.GetColumn(1)) * sizeY * -1.0f + position;
    Vector3 vertice2 = static_cast<Vector3>(modelViewMatrix.GetColumn(0)) * sizeX * 2.0f + vertice1;
    Vector3 vertice3 = static_cast<Vector3>(modelViewMatrix.GetColumn(1)) * sizeY * 2.0f + vertice2;
    Vector3 vertice4 = static_cast<Vector3>(modelViewMatrix.GetColumn(1)) * sizeY * 2.0f + vertice1;

    //setup draw mode, set color and draw particle
    vglBegin(gfxDrawMode::DRAWMODE_TRIANGLEFAN, 4);
    vglCurrentColor = color.PackColorBGRA();
    vglCreateVertex(0.0f, 0.0f, vertice1);
    vglCreateVertex(1.0f, 0.0f, vertice2);
    vglCreateVertex(1.0f, 1.0f, vertice3);
    vglCreateVertex(0.0f, 1.0f, vertice4);
    vglEnd();

    vglEndBatch();
    gfxRenderState::SetCullMode(previousCullMode);
}

void MM2::tglDrawCustomParticle(const Matrix34& matrix, dgBangerGlowData* glowData, gfxTexture* defaultGlowTexture, lvlInstance* instance)
{
    int index = glowData->FlashPerIndex ? glowData->Index : 0;
    int flatMode = glowData->FlatMode;
    int fadeStyle = glowData->FadeStyle;
    int enableFade = glowData->EnableFade;
    int invFlashTime = glowData->InvFlashTime;
    int fadeDirection = glowData->FadeDirection;
    int invFadeDirection = glowData->InvFadeDirection;

    Matrix34 modelMatrix = Matrix34();
    modelMatrix.Set(matrix);

    Vector3 drawPosition = modelMatrix.Transform(glowData->Offset);
    Vector3 cameraPosition = static_cast<Vector3>(gfxRenderState::GetCameraMatrix().GetRow(3));
    Vector3 positionDifference = cameraPosition - drawPosition;

    if (flatMode)
    {
        Vector3 angle = Vector3(glowData->FlatRotation * 0.017453f); // Degrees to Radians
        Vector3 rotationSpeed = Vector3(glowData->RotationSpeed * datTimeManager::ElapsedTime);
        modelMatrix.Rotate(modelMatrix.GetRow(0), angle.X + rotationSpeed.X);
        modelMatrix.Rotate(modelMatrix.GetRow(1), angle.Y + rotationSpeed.Y);
        modelMatrix.Rotate(modelMatrix.GetRow(2), angle.Z + rotationSpeed.Z);
    }

    float direction = (enableFade || flatMode) ? positionDifference.InvMag() * positionDifference.Dot(modelMatrix.GetRow(fadeDirection) * (invFadeDirection ? 1.0f : -1.0f)) : 1.0f;
    float size = fadeStyle ? fmaxf(glowData->Size, 0.0f) : fminf(fmaxf(direction, 0.0f), 1.0f) * fmaxf(glowData->Size, 0.0f);
    float intensity = (fadeStyle || flatMode) ? fminf(fmaxf(direction, 0.0f), 1.0f) : 1.0f;
    Vector4 color = glowData->Color * glowData->Color.W * intensity;

    gfxTexture* glowTexture = gfxGetTexture(glowData->Name, true);
    glowTexture != nullptr ? vglBindTexture(glowTexture) : vglBindTexture(defaultGlowTexture);

    Vector2 flashFreq = glowData->FlashFreq;
    bool flashTime = !flashFreq.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq.X) > flashFreq.Y);
    if ((flashTime && !invFlashTime) || (!flashTime && invFlashTime))
    {
        Vector2 flashFreq2 = glowData->FlashFreq2;
        bool flashTime2 = !flashFreq2.X || (fmodf(datTimeManager::ElapsedTime + (float)(index + instance->GetRandId()), flashFreq2.X) > flashFreq2.Y);
        if (flashTime2)
        {
            if (flatMode)
                tglDrawFlatParticle(modelMatrix, drawPosition, color, glowData, defaultGlowTexture);
            else
                tglDrawParticleClipAdjusted(drawPosition, size, color);
        }
    }
}

void MM2::tglDrawFlatParticle(const Matrix34& matrix, const Vector3& particlePosition, const Vector4& color, dgBangerGlowData* glowData, gfxTexture* defaultGlowTexture)
{
    //make flat particle double sided
    auto previousCullMode = gfxRenderState::GetCullMode();
    gfxRenderState::SetCullMode(D3DCULL_NONE);
    rglWorldIdentity();
    vglBeginBatch();

    //swap texture if it exists otherwise use the default one
    gfxTexture* glowTexture = gfxGetTexture(glowData->Name, true);
    glowTexture != nullptr ? vglBindTexture(glowTexture) : vglBindTexture(defaultGlowTexture);

    //fix particle clipping
    Vector3 particlePosAAA;
    tglParticleClipAdjust(particlePosition, particlePosAAA);

    //get size
    Vector2 size = glowData->FlatSize;

    //prepare vertex positions
    Vector3 position = (matrix.GetRow(0) * size.X * -1.0f) + particlePosAAA;
    Vector3 vertice1 = (matrix.GetRow(1) * size.Y * -1.0f) + position;
    Vector3 vertice2 = (matrix.GetRow(0) * size.X * 2.0f) + vertice1;
    Vector3 vertice3 = (matrix.GetRow(1) * size.Y * 2.0f) + vertice2;
    Vector3 vertice4 = (matrix.GetRow(1) * size.Y * 2.0f) + vertice1;

    //setup draw mode, set color and draw particle
    vglBegin(gfxDrawMode::DRAWMODE_TRIANGLEFAN, 4);
    vglCurrentColor = color.PackColorBGRA();
    vglCreateVertex(0.0f, 0.0f, vertice1);
    vglCreateVertex(1.0f, 0.0f, vertice2);
    vglCreateVertex(1.0f, 1.0f, vertice3);
    vglCreateVertex(0.0f, 1.0f, vertice4);
    vglEnd();

    vglEndBatch();
    gfxRenderState::SetCullMode(previousCullMode);
}

/* rglext.obj */

void    MM2::rglOpenPipe            (int p1, int p2, bool p3)                                   { return _StaticThunk<0x4A6970>::Call<void>(p1, p2, p3); }
void    MM2::rglClosePipe           (void)                                                      { return _StaticThunk<0x4A6A00>::Call<void>(); }
void    MM2::rglBeginFrame          (uint p1)                                                   { return _StaticThunk<0x4A6A10>::Call<void>(p1); }
void    MM2::rglEndFrame            (void)                                                      { return _StaticThunk<0x4A6A40>::Call<void>(); }
void    MM2::rglCameraMatrix        (const Matrix34 &p1)                                        { return _StaticThunk<0x4A6A50>::Call<void>(&p1); }
int     MM2::rglAlive               (void)                                                      { return _StaticThunk<0x4A6A70>::Call<int>(); }
void    MM2::rglDrawBox             (const Vector3 &p1, const Matrix34 &p2)                     { return _StaticThunk<0x4A6AB0>::Call<void>(&p1, &p2); }
void    MM2::rglDrawSolidBox        (const Vector3 &p1, const Matrix34 &p2, const Vector3 &p3)  { return _StaticThunk<0x4A6D30>::Call<void>(&p1, &p2, &p3); }
void    MM2::rglDrawAxis            (float p1, const Matrix34 &p2)                              { return _StaticThunk<0x4A7050>::Call<void>(p1, &p2); }
void    MM2::rglDrawSphere          (float p1, int p2)                                          { return _StaticThunk<0x4A70F0>::Call<void>(p1, p2); }
void    MM2::rglDrawSphere          (float p1, const Matrix34 &p2, int p3)                      { return _StaticThunk<0x4A7200>::Call<void>(p1, &p2, p3); }
void    MM2::rglDrawEllipsoid       (const Vector3 &p1, const Matrix34 &p2, int p3)             { return _StaticThunk<0x4A7230>::Call<void>(&p1, &p2, p3); }
void    MM2::rglDrawParticle        (const Vector3 &p1, float p2, const Vector4 &p3)            { return _StaticThunk<0x4A7310>::Call<void>(&p1, p2, &p3); }
void    MM2::rglResetDrawTexture    (void)                                                      { return _StaticThunk<0x4A7540>::Call<void>(); }
void    MM2::rglDrawTexture         (gfxTexture *texture)                                       { return _StaticThunk<0x4A7560>::Call<void>(texture); }
void    MM2::rglPushMatrix          (void)                                                      { return _StaticThunk<0x4A7750>::Call<void>(); }
void    MM2::rglPopMatrix           (void)                                                      { return _StaticThunk<0x4A7790>::Call<void>(); }
void    MM2::rglMultMatrix          (Matrix34 &p1)                                              { return _StaticThunk<0x4A77D0>::Call<void>(&p1); }