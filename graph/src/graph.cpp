#include <dmsdk/sdk.h>
#include <dmsdk/dlib/math.h>
#include <dmsdk/hid/hid.h>
#include "dump.h"

#include <stdlib.h> // realloc

#define MODULE_NAME Graph
#define LIB_NAME "graph"


namespace dmGraph
{

struct GraphContext
{
    char*               m_Buffer;
    uint32_t            m_BufferSize;
    uint32_t            m_BufferCapacity;
    bool                m_Initialized;

    dmGameObject::HRegister m_Register;
    dmHID::HContext         m_HidContext;
} g_Graph;

static int Graph_Dump(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);

    dmGraph::DumpToLuaTable(L, g_Graph.m_Register);

    return 1;
}

// Functions exposed to Lua
static const luaL_reg Graph_module_methods[] =
{
    {"dump", Graph_Dump},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    luaL_register(L, LIB_NAME, Graph_module_methods);
    lua_pop(L, 1);
}


static dmExtension::Result AppInitialize(dmExtension::AppParams* params)
{
    memset(&g_Graph, 0, sizeof(g_Graph));

    g_Graph.m_Register = dmEngine::GetGameObjectRegister(params);
    g_Graph.m_HidContext = dmEngine::GetHIDContext(params);
    g_Graph.m_Initialized = true;

    return dmExtension::RESULT_OK;
}


static dmExtension::Result Initialize(dmExtension::Params* params)
{
    if (!g_Graph.m_Initialized)
        return dmExtension::RESULT_OK;

    LuaInit(params->m_L);
    dmLogInfo("Registered %s extension", LIB_NAME);

    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalize(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result Finalize(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}


}

DM_DECLARE_EXTENSION(MODULE_NAME, LIB_NAME, dmGraph::AppInitialize, dmGraph::AppFinalize, dmGraph::Initialize, 0, 0, dmGraph::Finalize)

#undef LIB_NAME
