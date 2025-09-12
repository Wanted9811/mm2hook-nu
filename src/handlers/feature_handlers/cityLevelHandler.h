#pragma once
#include "common.h"
#include "patch.h"
#include "mm2.h"

class cityLevelHandler {
public:
    static int city_numRooms;
    static int city_currentRoom;

    static void UpdateLighting();

    static void LightmapDelete(void* data);

    void DrawRooms(const MM2::gfxViewport& viewport, unsigned int p2, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawSDL(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawStatics(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawPropShadows(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawProps(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawMoverShadows(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawMovers(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawAlpha(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawLights(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawReflected(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawStaticGlass(const MM2::gfxViewport& viewport, const MM2::cityRoomRec* roomRecs, int numRooms);
    void DrawParticles();
    void SetObjectDetail(int lod);

    MM2::gfxTexture* GetEnvMap(int roomNumber, MM2::Vector3 const &, float& intensity);

    static void Install();
};

