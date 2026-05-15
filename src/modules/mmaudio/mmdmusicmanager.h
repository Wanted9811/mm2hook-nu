#pragma once
#include <modules\directmusic\dmusicmanager.h>

namespace MM2
{
    // Forward declarations
    class MMDMusicManager;

    class MMDMusicManager : public DMusicManager {
    private:
        char buffer[0x50];
    protected:
        static hook::Field<0x24, int> _finalLapSegmentIndex;
        static hook::Field<0x2C, int> _idleSegmentIndex;
        static hook::Field<0x30, int> _idleChaseSegmentIndex;
        static hook::Field<0x34, int> _pauseSegmentIndex;
        static hook::Field<0x38, int> _returnSegmentIndex;
        static hook::Field<0x3C, int> _chaseSegmentIndex;
        static hook::Field<0x40, int> _resultsSegmentIndex;
        static hook::Field<0x44, int> _startSegmentIndex;
        static hook::Field<0x4C, int> _numCopsPursuingPlayer;
        static hook::Field<0x50, bool> _disableIdleSegmentSwitch;
        static hook::Field<0x51, bool> _isAirBorne;
    public:
        static hook::Type<MMDMusicManager*> Instance;

        int GetFinalLapSegmentIndex() const;
        int GetIdleSegmentIndex() const;
        int GetIdleChaseSegmentIndex() const;
        int GetPauseSegmentIndex() const;
        int GetReturnSegmentIndex() const;
        int GetChaseSegmentIndex() const;
        int GetResultsSegmentIndex() const;
        int GetStartSegmentIndex() const;

        int GetNumCopsPursuingPlayer() const;
        void SetNumCopsPursuingPlayer(int numCops);

        bool GetIdleSegmentSwitchEnabled() const;
        void SetIdleSegmentSwitchEnabled(bool enabled);

        bool GetIsAirBorne() const;
        void SetIsAirBorne(bool isAirBorne);

        void EchoOn(float delay, float volumeScale);

        void EchoOff();

        void MatchMusicToPlayerSpeed(float speed);

        void UpdateMusic(float speed, int numCops, bool isAirBorne);

        static void BindLua(LuaState L);
    };
}