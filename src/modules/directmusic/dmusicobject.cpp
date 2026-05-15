#pragma once
#include "DMusicObject.h"

namespace MM2
{
    void DMusicObject::BindLua(LuaState L) {
        LuaBinding(L).beginClass<DMusicObject>("DMusicObject")
            .addPropertyReadOnly("CurrentSegment", &GetCurrentSegment)
            .addPropertyReadOnly("IsPlaying", &IsPlaying)
            .addFunction("PlayMotif", &PlayMotif)
            .addFunction("PlaySegment", &PlaySegment)
            .addFunction("SetGrooveLevel", &SetGrooveLevel)
            .addFunction("SegmentSwitch", static_cast<int (DMusicObject::*)(int,int,int)>(&SegmentSwitch))
            .endClass();
    }
}