#include "mmdmusicmanager.h"
using namespace MM2;

/*
    MMDMusicManager
*/
declfield(MMDMusicManager::Instance)(0x6B15EC);

int MMDMusicManager::GetFinalLapSegmentIndex() const              { return _finalLapSegmentIndex.get(this); }
int MMDMusicManager::GetIdleSegmentIndex() const                  { return _idleSegmentIndex.get(this); }
int MMDMusicManager::GetIdleChaseSegmentIndex() const             { return _idleChaseSegmentIndex.get(this); }
int MMDMusicManager::GetPauseSegmentIndex() const                 { return _pauseSegmentIndex.get(this); }
int MMDMusicManager::GetReturnSegmentIndex() const                { return _returnSegmentIndex.get(this); }
int MMDMusicManager::GetChaseSegmentIndex() const                 { return _chaseSegmentIndex.get(this); }
int MMDMusicManager::GetResultsSegmentIndex() const               { return _resultsSegmentIndex.get(this); }
int MMDMusicManager::GetStartSegmentIndex() const                 { return _startSegmentIndex.get(this); }

int MMDMusicManager::GetNumCopsPursuingPlayer() const             { return _numCopsPursuingPlayer.get(this); }
void MMDMusicManager::SetNumCopsPursuingPlayer(int numCops)       { return _numCopsPursuingPlayer.set(this, numCops); }

bool MMDMusicManager::GetIdleSegmentSwitchEnabled() const         { return !_disableIdleSegmentSwitch.get(this); }
void MMDMusicManager::SetIdleSegmentSwitchEnabled(bool enabled)   { _disableIdleSegmentSwitch.set(this, !enabled); }

bool MMDMusicManager::GetIsAirBorne() const                       { return _isAirBorne.get(this); }
void MMDMusicManager::SetIsAirBorne(bool isAirBorne)              { _isAirBorne.set(this, isAirBorne); }

void MMDMusicManager::EchoOn(float delay, float volumeScale)
{
    hook::Thunk<0x51A2D0>::Call<void>(this, delay, volumeScale);
}

void MMDMusicManager::EchoOff()
{
    hook::Thunk<0x51A3A0>::Call<void>(this);
}

void MMDMusicManager::MatchMusicToPlayerSpeed(float speed)
{
    hook::Thunk<0x51A1D0>::Call<void>(this, speed);
}

void MMDMusicManager::UpdateMusic(float speed, int numCops, bool isAirBorne)
{
    MatchMusicToPlayerSpeed(speed);
    auto musicObject = GetDMusicObjectPtr();
    if (musicObject == nullptr)
        return;

    if (numCops > 0 && GetNumCopsPursuingPlayer() == 0)
    {
        musicObject->SegmentSwitch(GetFinalLapSegmentIndex());
    }
    else if (numCops == 0 && GetNumCopsPursuingPlayer() > 0)
    {
        musicObject->SegmentSwitch(GetReturnSegmentIndex());
    }

    SetNumCopsPursuingPlayer(numCops);

    if (isAirBorne)
    {
        if (!GetIsAirBorne())
        {
            musicObject->PlayMotif(0, 0x880);
            SetIsAirBorne(isAirBorne);
        }
    }
    else if (GetIsAirBorne())
    {
        SetIsAirBorne(false);
    }
}

void MMDMusicManager::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<MMDMusicManager, DMusicManager>("MMDMusicManager")
        .addPropertyReadOnly("FinalLapSegmentIndex", &GetFinalLapSegmentIndex)
        .addPropertyReadOnly("IdleSegmentIndex", &GetIdleSegmentIndex)
        .addPropertyReadOnly("IdleChaseSegmentIndex", &GetIdleChaseSegmentIndex)
        .addPropertyReadOnly("PauseSegmentIndex", &GetPauseSegmentIndex)
        .addPropertyReadOnly("ReturnSegmentIndex", &GetReturnSegmentIndex)
        .addPropertyReadOnly("ChaseSegmentIndex", &GetChaseSegmentIndex)
        .addPropertyReadOnly("ResultsSegmentIndex", &GetResultsSegmentIndex)
        .addPropertyReadOnly("StartSegmentIndex", &GetStartSegmentIndex)
        .addProperty("EnableIdleSegmentSwitch", &GetIdleSegmentSwitchEnabled, &SetIdleSegmentSwitchEnabled)
        .addProperty("NumCopsPursuingPlayer", &GetNumCopsPursuingPlayer, &SetNumCopsPursuingPlayer)
        .addProperty("IsAirBorne", &GetIsAirBorne, &SetIsAirBorne)
        .addFunction("EchoOn", &EchoOn)
        .addFunction("EchoOn", &EchoOff)
        .endClass();
}