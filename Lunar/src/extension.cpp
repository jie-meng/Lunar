#include "extension.h"
#include <vector>
#include "util/file.hpp"
#include "util/lexicalcast.hpp"
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

bool Extension::init()
{
    if (!isPathDir(LunarGlobal::getInstance().getAppPath() + "/plugins"))
        mkDir(LunarGlobal::getInstance().getAppPath() + "/plugins");

    return initLuaState();
}

bool Extension::initLuaState()
{
    if (lua_state_ok_)
        return true;

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
        lua_state_.registerFunction("messageBox", scriptMessage);
        error_information_ = "";
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

bool Extension::parseFilename(const std::string& filename,
               std::string* pout_type,
               size_t* pauto_complete_type_,
               std::string* pout_api,
               std::string* pout_executor,
               std::string* pout_parse_supplement_api_script,
               std::string* pout_parse_supplement_api_func)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncParseFileType());
    luaPushString(lua_state_.getState(), filename);

    int err = luaCallFunc(lua_state_.getState(), 1, 1);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        LunarMsgBox(error_information_);
        luaPop(lua_state_.getState(), -1);

        return false;
    }
    else
    {
        if (luaGetTop(lua_state_.getState()) > 0 && luaGetType(lua_state_.getState(), 1) == LuaTable)
        {
            std::vector< pair<any, any> > vec = luaGetTable(lua_state_.getState(), 1);
            std::map<string, string> tb;
            for (size_t i = 0; i<vec.size(); ++i)
                tb[vec[i].first.toString()] = vec[i].second.toString();

            if (pout_type != NULL && tb.find("type") != tb.end())
                *pout_type = tb["type"];

            if (pauto_complete_type_ != NULL && tb.find("auto_complete_type") != tb.end())
                *pauto_complete_type_ = lexicalCastDefault<size_t>(tb["auto_complete_type"], 0);

            if (pout_api != NULL && tb.find("api") != tb.end())
                *pout_api = tb["api"];

            if (pout_executor != NULL && tb.find("executor") != tb.end())
                *pout_executor = tb["executor"];

            if (pout_parse_supplement_api_script != NULL && tb.find("parse_supplement_api_script") != tb.end())
                *pout_parse_supplement_api_script = tb["parse_supplement_api_script"];

            if (pout_parse_supplement_api_func != NULL && tb.find("parse_supplement_api_func") != tb.end())
                *pout_parse_supplement_api_func = tb["parse_supplement_api_func"];
        }

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
        LunarMsgBox(error_information_);
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
            luaPop(lua_state_.getState(), -1);
            return "";
        }
    }
}

bool Extension::ignoreFile(const std::string& filename)
{
    if (!lua_state_ok_)
        return false;

    error_information_ = "";

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncIgnoreFile());
    luaPushString(lua_state_.getState(), filename);

    int err = luaCallFunc(lua_state_.getState(), 1, 1);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        LunarMsgBox(error_information_);
        luaPop(lua_state_.getState(), -1);

        return false;
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());
        if (ret_cnt > 0)
        {
            bool ret = luaGetBoolean(lua_state_.getState(), 1, false);
            luaPop(lua_state_.getState(), -1);
            return ret;
        }
        else
        {
            luaPop(lua_state_.getState(), -1);
            return false;
        }
    }
}
