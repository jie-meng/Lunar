#include "lregexlib.hpp"
#include "util/luaextend.hpp"
#include "util/regex.hpp"

namespace util
{

static int create(lua_State* plua_state)
{
    Regex* pregexer = new Regex(luaGetString(plua_state, 1, ""),
        (Regex::RegexFlag)luaGetInteger(plua_state, 2, 0));

//    LuaHeapRecyclerManager::getInstance().addHeapObject(plua_state, (void*)pregexer, deleteVoid<Regex>);
    LuaHeapRecyclerManager::getInstance().addHeapObject<Regex>(plua_state, (void*)pregexer);

    luaPushLightUserData(plua_state, (void*)pregexer);

    return 1;
}

static int destrory(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));

    LuaHeapRecyclerManager::getInstance().removeHeapObject(plua_state, (void*)pregexer);

    if (pregexer)
        delete pregexer;

    return 0;
}

static int compile(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "compile", pregexer,
        "null pointer");

    pregexer->compile(luaGetString(plua_state, 2));

    return 0;
}

static int match(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "match", pregexer,
        "null pointer");

    luaPushBoolean(plua_state, pregexer->match(luaGetString(plua_state, 2)));

    return 1;
}

static int search(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "search", pregexer,
        "null pointer");

    luaPushBoolean(plua_state, pregexer->search(luaGetString(plua_state, 2)));

    return 1;
}

static int getMatchedGroupCnt(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "getMatchedGroupCnt", pregexer,
        "null pointer");

    luaPushInteger(plua_state, pregexer->getMatchedGroupCnt());

    return 1;
}

static int getMatchedGroupByIndex(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "getMatchedGroupByIndex", pregexer,
        "null pointer");

    luaPushString(plua_state, pregexer->getMatchedGroup(luaGetInteger(plua_state, 2)));

    return 1;
}

static int getMatchedGroupByName(lua_State* plua_state)
{
    Regex* pregexer = static_cast<Regex*>(luaGetLightUserData(plua_state, 1, 0));
    luaExtendAssert(plua_state, kLuaExtendLibRegex, "getMatchedGroupByName", pregexer,
        "null pointer");

    luaPushString(plua_state, pregexer->getMatchedGroup(luaGetString(plua_state, 2)));

    return 1;
}

static const u_luaL_Reg regex_lib[] =
{
    {"create", create},
    {"destroy", destrory},
    {"compile", compile},
    {"match", match},
    {"search", search},
    {"getMatchedGroupCnt", getMatchedGroupCnt},
    {"getMatchedGroupByIndex", getMatchedGroupByIndex},
    {"getMatchedGroupByName", getMatchedGroupByName},

    {0, 0}
};

/*
** Open regex library
*/
int lualibRegexCreate(lua_State* plua_state) {

    luaCreateLib(plua_state, (u_luaL_Reg*)regex_lib);
    return 1;
}

}

