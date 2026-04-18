#include "fixed.h"

using namespace MM2;

/*
  lvlFixedAny
*/

lvlFixedAny::lvlFixedAny() 
{
    scoped_vtable x(this);
    hook::Thunk<0x467300>::Call<void>(this);
}

void lvlFixedAny::SetVariant(int variant)
{
    hook::Thunk<0x467540>::Call<void>(this, variant);
}

void lvlFixedAny::Draw(int lod)
{
    hook::Thunk<0x467420>::Call<void>(this, lod);
}

void lvlFixedAny::DrawShadow()          
{
    hook::Thunk<0x4677E0>::Call<void>(this); 
}

void lvlFixedAny::DrawShadowMap()     
{
    hook::Thunk<0x467740>::Call<void>(this); 
}

void lvlFixedAny::DrawReflectedParts(int lod)
{
    hook::Thunk<0x467320>::Call<void>(this, lod);
}

int lvlFixedAny::Init(const char* basename, const Matrix34& matrix, int flags)
{
    return hook::Thunk<0x467580>::Call<int>(this, basename, &matrix, flags);
}

void lvlFixedAny::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<lvlFixedAny, lvlInstance>("lvlFixedAny")
        .endClass();
}

/*
  lvlFixedRotY
*/

lvlFixedRotY::lvlFixedRotY()
{
    scoped_vtable x(this);
    hook::Thunk<0x467910>::Call<void>(this);
}

const Vector3& lvlFixedRotY::GetPosition()
{
    return hook::Thunk<0x467940>::Call<const Vector3&>(this);
}

int lvlFixedRotY::IsVisible(const gfxViewport& viewport)
{
    return hook::Thunk<0x467950>::Call<int>(this, &viewport);
}

const Matrix34& lvlFixedRotY::GetMatrix(Matrix34 &matrix)
{
    return hook::Thunk<0x4679A0>::Call<const Matrix34&>(this, &matrix);
}

void lvlFixedRotY::SetMatrix(const Matrix34& matrix)
{
    hook::Thunk<0x4679F0>::Call<void>(this, &matrix);
}

unsigned int lvlFixedRotY::SizeOf()
{
    return sizeof(lvlFixedRotY);
}

void lvlFixedRotY::BindLua(LuaState L) {
    LuaBinding(L).beginExtendClass<lvlFixedRotY, lvlFixedAny>("lvlFixedRotY")
        .addFactory([]() {
        return new lvlFixedRotY();
        })
        .endClass();
}

/*
  lvlFixedMatrix
*/

lvlFixedMatrix::lvlFixedMatrix() 
{
    scoped_vtable x(this);
    hook::Thunk<0x467A20>::Call<void>(this);
}

const Vector3& lvlFixedMatrix::GetPosition()
{
    return hook::Thunk<0x467A50>::Call<const Vector3&>(this);
}

int lvlFixedMatrix::IsVisible(gfxViewport const& viewport)
{
    return hook::Thunk<0x467A60>::Call<int>(this, &viewport);
}

const Matrix34& lvlFixedMatrix::GetMatrix(Matrix34& matrix)
{
    return hook::Thunk<0x467AB0>::Call<const Matrix34&>(this, &matrix);
}

void lvlFixedMatrix::SetMatrix(const Matrix34& matrix)
{
    hook::Thunk<0x467AC0>::Call<void>(this, &matrix);
}

unsigned int lvlFixedMatrix::SizeOf() 
{
    return sizeof(lvlFixedMatrix);
}

void lvlFixedMatrix::BindLua(LuaState L) 
{
    LuaBinding(L).beginExtendClass<lvlFixedMatrix, lvlFixedAny>("lvlFixedMatrix")
        .addFactory([]() {
        return new lvlFixedMatrix();
        })
        .endClass();
}