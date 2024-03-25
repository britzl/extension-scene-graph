#include "dump.h"
#include <dmsdk/gameobject/gameobject.h>
#include <dmsdk/dlib/math.h>

namespace dmGraph
{
    static void OutputProperty(lua_State* L, dmGameObject::SceneNodeProperty* property)
    {
        static dmhash_t hash_id = dmHashString64("id");
        static dmhash_t hash_name = dmHashString64("name");
        static dmhash_t hash_world_position = dmHashString64("world_position");
        static dmhash_t hash_world_size = dmHashString64("world_size");
        static dmhash_t hash_position = dmHashString64("position");
        static dmhash_t hash_pos = dmHashString64("pos");
        static dmhash_t hash_size = dmHashString64("size");

        dmhash_t name = property->m_NameHash;

        lua_pushstring(L, dmHashReverseSafe64(name));

        switch(property->m_Type)
        {
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_NUMBER:
            lua_pushnumber(L, property->m_Value.m_Number);
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_BOOLEAN:
            lua_pushboolean(L, property->m_Value.m_Bool);
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_URL:
            lua_pushstring(L, property->m_Value.m_URL);
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_TEXT:
            lua_pushstring(L, property->m_Value.m_Text);
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_VECTOR3:
            dmScript::PushVector3(L, dmVMath::Vector3(property->m_Value.m_V4[0], property->m_Value.m_V4[1], property->m_Value.m_V4[2]));
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_VECTOR4:
            dmScript::PushVector4(L, dmVMath::Vector4(property->m_Value.m_V4[0], property->m_Value.m_V4[1], property->m_Value.m_V4[2], property->m_Value.m_V4[3]));
            break;
        case dmGameObject::SCENE_NODE_PROPERTY_TYPE_QUAT:
            dmScript::PushQuat(L, dmVMath::Quat(property->m_Value.m_V4[0], property->m_Value.m_V4[1], property->m_Value.m_V4[2], property->m_Value.m_V4[3]));
            break;
        default:
            lua_pushnil(L); break;
        }

        lua_settable(L, -3);
    }

    static void GetNodeInfo(dmGameObject::SceneNode* node, dmhash_t& name)
    {
        static dmhash_t hash_id = dmHashString64("id");
        static dmhash_t hash_type = dmHashString64("type");
        static dmhash_t hash_world_position = dmHashString64("world_position");
        static dmhash_t hash_position = dmHashString64("position");

        dmGameObject::SceneNodePropertyIterator pit = TraverseIterateProperties(node);
        while(dmGameObject::TraverseIteratePropertiesNext(&pit))
        {
            if (pit.m_Property.m_NameHash == hash_id)
            {
                name = pit.m_Property.m_Value.m_Hash;
            }
        }
    }

    static void SceneGraphToLua(lua_State* L, dmGameObject::SceneNode* node)
    {
        static dmhash_t hash_id = dmHashString64("id");

        dmhash_t name = 0;
        GetNodeInfo(node, name);

        lua_pushstring(L, dmHashReverseSafe64(name));
        lua_setfield(L, -2, "name");

        lua_pushstring(L, "payload");
        lua_newtable(L);

            dmGameObject::SceneNodePropertyIterator pit = TraverseIterateProperties(node);
            while(dmGameObject::TraverseIteratePropertiesNext(&pit))
            {
                OutputProperty(L, &pit.m_Property);
            }

        lua_settable(L, -3);

        lua_pushstring(L, "children");
        lua_newtable(L);

            int counter = 0;
            dmGameObject::SceneNodeIterator it = dmGameObject::TraverseIterateChildren(node);
            while(dmGameObject::TraverseIterateNext(&it))
            {
                lua_pushinteger(L, 1+counter++);
                lua_newtable(L);

                    SceneGraphToLua(L, &it.m_Node);

                lua_settable(L, -3);
            }

        lua_settable(L, -3);
    }

    void DumpToLuaTable(lua_State* L, dmGameObject::HRegister regist)
    {
        DM_LUA_STACK_CHECK(L, 1);

        dmGameObject::SceneNode root;
        if (!dmGameObject::TraverseGetRoot(regist, &root))
        {
            lua_pushnil(L);
            return;
        }

        lua_newtable(L);
        dmVMath::Matrix4 identity = dmVMath::Matrix4::identity();
        SceneGraphToLua(L, &root);
    }
}