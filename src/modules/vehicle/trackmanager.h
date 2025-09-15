#pragma once
#include <modules\rgl.h>

namespace MM2
{
    // Forward declarations
    class lvlTrackManager;

    // External declarations


    // Class definitions
    struct lvlTrackData
    {
        Vector3 VertStartPosition;
        Vector3 VertEndPosition;
        gfxTexture* TireTrackTexture;
        float TextureWidth;
    };

    class lvlTrackManager {
    private:
        int MaxNumTracks;
        int MinNumTracks;
        int NumTracks;
        float HalfWheelWidth;
        float OneDivWheelWidth;
        lvlTrackData* TracksData;
        gfxTexture* TireTrackTexture;
        Vector3 IntersecPoint;
        Vector3 IntersecPointDiff;
        Vector3 VertStartPosition;
        Vector3 VertEndPosition;
        bool HasTexture;
        Matrix34* WheelMatrix;
        bool field_54;
    public:
        AGE_API lvlTrackManager();
        AGE_API ~lvlTrackManager();
        AGE_API void Init(Matrix34 const& wheelMatrix, float wheelWidth, int numTracks);
        AGE_API void Reset();
        AGE_API void Update(Vector3 const& intersecPoint, Vector3 const& intersecNormal, gfxTexture* tireTrackTexture);
        AGE_API void Draw();
        AGE_API void AddVertex(gfxTexture* tireTrackTexture, float textureWidth, Vector3 const& startPosition, Vector3 const& endPosition);

        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(lvlTrackManager, 0x58);
}