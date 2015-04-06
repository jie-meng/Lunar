#include "extension.h"
#include "util/file.hpp"
#include "util/luaextend.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

Extension::Extension() :
    lua_state_ok_(false),
    error_information_("")
{
}

Extension::~Extension()
{
}

bool Extension::initLuaState()
{
    if (!isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile()))
    {
        error_information_ =
            strFormat("Extension: extension file %s not exist", (LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile()).c_str());
        lua_state_ok_ = false;
        return false;
    }

    openUtilExtendLibs(lua_state_.getState());

    int err = lua_state_.parseFile(LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionFile());
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        lua_state_ok_ = false;
    }
    else
    {
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

bool Extension::parse(const std::string& filename,
               std::string* pout_type,
               std::string* pout_api,
               std::string* pout_executor)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncParseFileType());
    luaPushString(lua_state_.getState(), filename);

    int err = luaCallFunc(lua_state_.getState(), 1, 3);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        luaPop(lua_state_.getState(), -1);

        return false;
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());

        if (ret_cnt > 0 && pout_type != NULL)
            *pout_type = luaGetString(lua_state_.getState(), 1);

        if (ret_cnt > 1 && pout_api != NULL)
            *pout_api = luaGetString(lua_state_.getState(), 2);

        if (ret_cnt > 2 && pout_executor != NULL)
            *pout_executor = luaGetString(lua_state_.getState(), 3);

        error_information_ = "";
        luaPop(lua_state_.getState(), -1);

        return true;
    }
}
