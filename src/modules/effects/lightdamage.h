#pragma once
#include <modules/model/static2.h>

namespace MM2
{
    // Forward declarations
    class mmLightDamage;

    // External declarations


    // Class definitions
    class mmLightDamage
    {
    public:
        static const int LightCount = 10;
    public:
        enum LightID
        {
            HLIGHT,
            TLIGHT,
            RLIGHT,
            SLIGHT0,
            SLIGHT1,
            BLIGHT,
            SIREN0,
            SIREN1,
            TSLIGHT0,
            TSLIGHT1,
        };
    private:
        int VertexCount[LightCount];
        modStatic* Model[LightCount];
        modStatic* LightModel[LightCount];
        Vector3* VertexPositions[LightCount];
    public:
        ANGEL_ALLOCATOR

        mmLightDamage();

        virtual ~mmLightDamage();

        void Init(modStatic* model, int id);

        void Reset(bool showDamage);

        void Apply(Vector3& impactPosition, float radius);

        modStatic* GetLightModel(int id) const;
    };
}