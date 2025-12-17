#include "data.h"

using namespace MM2;

/*
    dgBangerData
*/

Vector3 dgBangerData::GetGlowOffset(int num) const
{
    if (num >= this->NumGlows || num < 0)
        return Vector3::ORIGIN;
    return Vector3(this->GlowDatas[num].Offset);
}

void dgBangerData::SetGlowOffset(int num, Vector3 offset)
{
    if (num >= this->NumGlows || num < 0)
        return;
    (&this->GlowDatas[num].Offset)->Set(offset);
}

void dgBangerData::InitBound()
{
    hook::Thunk<0x4411C0>::Call<void>(this);
}

/*
    asNode virtuals
*/

AGE_API bool dgBangerData::Save()
{
    auto name = this->GetName();
    auto dirName = this->GetDirName();
    auto className = this->GetClassName();

    if (!strcmp(name, ""))
        strcpy(name, "default");

    auto stream = datAssetManager::Create(dirName, name, className, false);
    if (!stream)
        return false;

    datMultiTokenizer multiTokenizer = datMultiTokenizer();
    datBaseTokenizer* writeTokenizer = &multiTokenizer.GetWriteTokenizer(name, stream, NULL, "type: a");

    writeTokenizer->Put(className, 0);
    writeTokenizer->Put(" {\n  ", 0);

    writeTokenizer->PutToken("AudioId ", this->AudioId);
    writeTokenizer->PutToken("Size ", this->Size);
    writeTokenizer->PutToken("CG ", this->CG);
    writeTokenizer->PutToken("NumGlows ", this->NumGlows);

    for (int i = 0; i < this->NumGlows; i++)
        writeTokenizer->PutToken("GlowOffset ", this->GlowDatas[i].Offset);

    writeTokenizer->PutToken("Mass ", this->Mass);
    writeTokenizer->PutToken("Elasticity ", this->Elasticity);
    writeTokenizer->PutToken("Friction ", this->Friction);
    writeTokenizer->PutToken("ImpulseLimit2 ", this->ImpulseLimit2);
    writeTokenizer->PutToken("SpinAxis ", this->SpinAxis);
    writeTokenizer->PutToken("Flash ", this->Flash);
    writeTokenizer->PutToken("NumParts ", this->NumParts);

    this->BirthRule.Save(writeTokenizer, 0x4);
    writeTokenizer->Put("\n  ", 0);

    writeTokenizer->PutToken("TexNumber ", this->TexNumber);
    writeTokenizer->PutToken("BillFlags ", this->BillFlags);
    writeTokenizer->PutToken("YRadius ", this->YRadius);
    writeTokenizer->PutToken("ColliderId ", this->ColliderId);
    writeTokenizer->PutToken("CollisionPrim ", this->CollisionPrim);
    writeTokenizer->PutToken("CollisionType ", this->CollisionType);

    writeTokenizer->Put("}\n", 0);
    stream->Close();

    /*
        dgBangerGlowData
    */

    char* className2 = "dgBangerGlowData";
    auto stream2 = datAssetManager::Create(dirName, name, className2, false);
    if (stream2)
    {
        datMultiTokenizer multiTokenizer = datMultiTokenizer();
        datBaseTokenizer* writeTokenizer = &multiTokenizer.GetWriteTokenizer(name, stream2, NULL, "type: a");

        writeTokenizer->Put(className2, 0);
        writeTokenizer->Put(" {\n  ", 0);

        for (int i = 0; i < this->NumGlows; i++)
        {
            auto glowData = &this->GlowDatas[i];

            string_buf<32> LightGlowId("LightGlow%d", glowData->Index);
            writeTokenizer->Put(LightGlowId, 0);
            writeTokenizer->Put(" {\n    ", 0);

            writeTokenizer->PutToken("GlowName ", glowData->Name);
            writeTokenizer->PutToken("GlowColor ", glowData->Color);
            writeTokenizer->PutToken("GlowSize ", glowData->Size);
            writeTokenizer->PutToken("FlatMode ", glowData->FlatMode);
            writeTokenizer->PutToken("FlatSize ", glowData->FlatSize);
            writeTokenizer->PutToken("FlatRotation ", glowData->FlatRotation);
            writeTokenizer->PutToken("RotationSpeed ", glowData->RotationSpeed);
            writeTokenizer->PutToken("EnableFade ", glowData->EnableFade);
            writeTokenizer->PutToken("FadeStyle ", glowData->FadeStyle);
            writeTokenizer->PutToken("FadeDirection ", glowData->FadeDirection);
            writeTokenizer->PutToken("InvFadeDirection ", glowData->InvFadeDirection);
            writeTokenizer->PutToken("FlashFreq ", glowData->FlashFreq);
            writeTokenizer->PutToken("FlashFreq2 ", glowData->FlashFreq2);
            writeTokenizer->PutToken("InvFlashTime ", glowData->InvFlashTime);
            writeTokenizer->PutToken("FlashPerIndex ", glowData->FlashPerIndex);
            writeTokenizer->PutToken("EnableShadow ", glowData->EnableShadow);
            writeTokenizer->PutToken("ShadowSize ", glowData->ShadowSize);
            writeTokenizer->PutToken("ShadowIntensity ", glowData->ShadowIntensity);
            writeTokenizer->PutToken("EnableProjection ", glowData->EnableProjection);

            writeTokenizer->Put(" }\n", 0);
        }

        writeTokenizer->Put("}\n", 0);
        stream2->Close();
    }
    return true;
}

AGE_API bool dgBangerData::Load()
{
    auto name = this->GetName();
    auto dirName = this->GetDirName();
    auto className = this->GetClassName();

    if (!strcmp(name, ""))
        strcpy(name, "default");

    bool exists = datAssetManager::Exists(dirName, name, className);
    if (!exists)
        return false;

    auto stream = datAssetManager::Open(dirName, name, className, false, true);
    if (!stream)
        return false;

    datMultiTokenizer multiTokenizer = datMultiTokenizer();
    datBaseTokenizer* readTokenizer = &multiTokenizer.GetReadTokenizer(name, stream, "type: a", "type: b");

    char buf[128];
    readTokenizer->GetToken(buf, 128);
    readTokenizer->GetToken(buf, 128);

    this->AudioId = readTokenizer->MatchInt("AudioId");
    readTokenizer->MatchVector("Size", this->Size);
    readTokenizer->MatchVector("CG", this->CG);

    if (readTokenizer->CheckToken("NumGlows", false))
    {
        readTokenizer->IgnoreToken();
        this->NumGlows = readTokenizer->GetInt();
    }
    else
    {
        this->NumGlows = 1;
    }

    int numGlows = this->NumGlows;
    this->GlowDatas = new dgBangerGlowData[numGlows];

    for (int i = 0; i < this->NumGlows; i++)
        readTokenizer->MatchVector("GlowOffset", this->GlowDatas[i].Offset);

    readTokenizer->IgnoreToken();
    this->Mass = readTokenizer->GetFloat();
    readTokenizer->IgnoreToken();
    this->Elasticity = readTokenizer->GetFloat();
    readTokenizer->IgnoreToken();
    this->Friction = readTokenizer->GetFloat();
    readTokenizer->IgnoreToken();
    this->ImpulseLimit2 = readTokenizer->GetFloat();
    readTokenizer->IgnoreToken();
    this->SpinAxis = readTokenizer->GetInt();
    readTokenizer->IgnoreToken();
    this->Flash = readTokenizer->GetInt();
    readTokenizer->IgnoreToken();
    this->NumParts = readTokenizer->GetInt();

    this->BirthRule.Load(readTokenizer);

    readTokenizer->IgnoreToken();
    this->TexNumber = readTokenizer->GetInt();
    readTokenizer->IgnoreToken();
    this->BillFlags = readTokenizer->GetInt();
    readTokenizer->IgnoreToken();
    this->YRadius = readTokenizer->GetFloat();

    if (readTokenizer->CheckToken("ColliderId", true))
        this->ColliderId = readTokenizer->GetInt();
    if (readTokenizer->CheckToken("CollisionPrim", true))
        this->CollisionPrim = readTokenizer->GetInt();
    if (readTokenizer->CheckToken("CollisionType", true))
        this->CollisionType = readTokenizer->GetInt();

    stream->Close();

    if (strstr(this->GetName(), "_tree"))
        this->BillFlags |= 0x200;

    for (int i = 0; i < this->NumGlows; i++)
        if (strstr(this->GetName(), "light")) this->GlowDatas[i].EnableProjection = 1;

    /*
        dgBangerGlowData
    */

    char* className2 = "dgBangerGlowData";
    bool exists2 = datAssetManager::Exists(dirName, name, className2);
    if (exists2)
    {
        auto stream2 = datAssetManager::Open(dirName, name, className2, false, true);
        if (stream2)
        {
            datMultiTokenizer multiTokenizer = datMultiTokenizer();
            datBaseTokenizer* readTokenizer = &multiTokenizer.GetReadTokenizer(name, stream2, "type: a", "type: b");

            char buf[128];
            readTokenizer->GetToken(buf, 128);
            readTokenizer->GetToken(buf, 128);

            for (int i = 0; i < this->NumGlows; i++)
            {
                auto glowData = &this->GlowDatas[i];
                glowData->Index = i;

                string_buf<32> LightGlowId("LightGlow%d", glowData->Index);
                if (readTokenizer->CheckToken(LightGlowId, true))
                {
                    readTokenizer->IgnoreToken();
                    readTokenizer->AddToken("GlowName", glowData->Name, 64);
                    readTokenizer->AddToken("GlowColor", glowData->Color);
                    readTokenizer->AddToken("GlowSize", glowData->Size);
                    readTokenizer->AddToken("FlatMode", glowData->FlatMode);
                    readTokenizer->AddToken("FlatSize", glowData->FlatSize);
                    readTokenizer->AddToken("FlatRotation", glowData->FlatRotation);
                    readTokenizer->AddToken("RotationSpeed", glowData->RotationSpeed);
                    readTokenizer->AddToken("EnableFade", glowData->EnableFade);
                    readTokenizer->AddToken("FadeStyle", glowData->FadeStyle);
                    readTokenizer->AddToken("FadeDirection", glowData->FadeDirection);
                    readTokenizer->AddToken("InvFadeDirection", glowData->InvFadeDirection);
                    readTokenizer->AddToken("FlashFreq", glowData->FlashFreq);
                    readTokenizer->AddToken("FlashFreq2", glowData->FlashFreq2);
                    readTokenizer->AddToken("InvFlashTime", glowData->InvFlashTime);
                    readTokenizer->AddToken("FlashPerIndex", glowData->FlashPerIndex);
                    readTokenizer->AddToken("EnableShadow", glowData->EnableShadow);
                    readTokenizer->AddToken("ShadowSize", glowData->ShadowSize);
                    readTokenizer->AddToken("ShadowIntensity", glowData->ShadowIntensity);
                    readTokenizer->AddToken("EnableProjection", glowData->EnableProjection);
                    readTokenizer->IgnoreToken();
                }
            }

            stream2->Close();
        }
    }

    return true;
}

AGE_API char* dgBangerData::GetClassName()
{
    return hook::Thunk<0x441520>::Call<char*>(this);
}

AGE_API char const* dgBangerData::GetDirName()
{
    return hook::Thunk<0x440600>::Call<char const*>(this);
}

void dgBangerData::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgBangerData, asNode>("dgBangerData")
        .addVariable("Index", &dgBangerData::Index, false)
        .addVariable("NumGlows", &dgBangerData::NumGlows, false)
        .addVariable("NumParts", &dgBangerData::NumParts, false)
        .addVariable("GeometrySetIndex", &dgBangerData::GeomSet, false)
        .addVariableRef("BirthRule", &dgBangerData::BirthRule, false)

        .addVariable("CG", &dgBangerData::CG)
        .addVariable("Size", &dgBangerData::Size)
        .addVariable("Mass", &dgBangerData::Mass)
        .addVariable("Elasticity", &dgBangerData::Elasticity)
        .addVariable("Friction", &dgBangerData::Friction)
        .addVariable("ImpulseLimit2", &dgBangerData::ImpulseLimit2)
        .addVariable("YRadius", &dgBangerData::YRadius)
        .addVariable("ColliderId", &dgBangerData::ColliderId)
        .addVariable("TexNumber", &dgBangerData::TexNumber)
        .addVariable("BillFlags", &dgBangerData::BillFlags)
        .addVariable("SpinAxis", &dgBangerData::SpinAxis)
        .addVariable("Flash", &dgBangerData::Flash)
        .addVariable("CollisionType", &dgBangerData::CollisionType)
        .addVariable("CollisionPrim", &dgBangerData::CollisionPrim)
        .addVariable("AudioId", &dgBangerData::AudioId)
        .addVariableRef("Bound", &dgBangerData::Bound)

        .addFunction("GetGlowOffset", &GetGlowOffset)
        .addFunction("SetGlowOffset", &SetGlowOffset)
    .endClass();
}

/*
    dgBangerDataManager
*/

int dgBangerDataManager::GetDataCount() const
{
    return dataCount;
}

dgBangerData* dgBangerDataManager::GetData(int id)
{
    if (id >= dataCount || id < 0)
        return nullptr;
    return &datas[id];
}

dgBangerDataManager* dgBangerDataManager::GetInstance()
{
    return Instance.get();
}

/*
    asNode virtuals
*/

AGE_API char* dgBangerDataManager::GetClassName()
{
    return hook::Thunk<0x4415B0>::Call<char*>(this);
}

/*
    dgBangerDataManager
*/

AGE_API int dgBangerDataManager::AddBangerDataEntry(char const* name, char const* partName)
{
    return hook::Thunk<0x440940>::Call<int>(this, name, partName);
}

AGE_API void dgBangerDataManager::ChangeData()
{
    hook::Thunk<0x440A60>::Call<void>(this);
}

void dgBangerDataManager::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<dgBangerDataManager, asNode>("dgBangerDataManager")
        .addStaticProperty("Instance", &dgBangerDataManager::GetInstance)
        .addFunction("AddBangerDataEntry", &AddBangerDataEntry, LUA_ARGS(LPCSTR, _opt<LPCSTR>))
    .endClass();
}