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

bool Extension::parseFilename(const std::string& filename,
               std::string* pout_type,
               std::string* pout_api,
               std::string* pout_executor,
               std::string* pout_parse_supplement_api_script,
               std::string* pout_parse_supplement_api_func)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncParseFileType());
    luaPushString(lua_state_.getState(), filename);

    int err = luaCallFunc(lua_state_.getState(), 1, 5);
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

        if (ret_cnt > 3 && pout_parse_supplement_api_script != NULL)
            *pout_parse_supplement_api_script = luaGetString(lua_state_.getState(), 4);

        if (ret_cnt > 4 && pout_parse_supplement_api_func != NULL)
            *pout_parse_supplement_api_func = luaGetString(lua_state_.getState(), 5);

        error_information_ = "";
        luaPop(lua_state_.getState(), -1);

        return true;
    }
}

std::string Extension::fileFilter()
{
    if (!lua_state_ok_)
        return "";

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncFilefilter());

    int err = luaCallFunc(lua_state_.getState(), 0, 1);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        luaPop(lua_state_.getState(), -1);

        return "";
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());
        if (ret_cnt > 0)
        {
            string result = "";

            if(luaGetType(lua_state_.getState(), 1) == LuaTable)
            {
                vector< pair<any, any> > vec = luaGetTable(lua_state_.getState(), 1);
                vector< pair<any, any> >::iterator it;
                for (it=vec.begin(); it != vec.end(); ++it)
                {
                    result += it->second.toString() + ";;";
                }
            }
            else if (luaGetType(lua_state_.getState(), 1) == LuaString)
            {
                result = luaGetString(lua_state_.getState(), 1);
                error_information_ = "";
            }

            luaPop(lua_state_.getState(), -1);
            return result;
        }
        else
        {
            return "";
        }
    }
}
