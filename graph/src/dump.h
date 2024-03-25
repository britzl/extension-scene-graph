#pragma once

#include <dmsdk/sdk.h>

namespace dmGameObject
{
    /// Component type and game object register
    typedef struct Register* HRegister;
}

namespace dmGraph
{
    void DumpToLuaTable(lua_State* L, dmGameObject::HRegister regist);
}