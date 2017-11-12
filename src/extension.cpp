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
        lua_state_.registerFunction("sendLog", sendLog);
        error_information_ = "";
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

bool Extension::parseFilename(const std::string& filename, std::map<std::string, std::string>& out_map)
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
            std::vector< pair<any, any> > vec = luaToArray(lua_state_.getState(), 1);
            for (size_t i = 0; i<vec.size(); ++i)
                out_map[vec[i].first.toString()] = vec[i].second.toString();
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

            if (luaGetType(lua_state_.getState(), 1) == LuaTable)
            {
                vector< pair<any, any> > vec = luaToArray(lua_state_.getState(), 1);
                vector< pair<any, any> >::iterator it;
                for (it=vec.begin(); it != vec.end(); ++it)
                {
                    result += it->second.toString() + ";;";
                }
            }
            else if (luaGetType(lua_state_.getState(), 1) == LuaString)
            {
                result = luaToString(lua_state_.getState(), 1);
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

bool Extension::isLegalFile(const std::string& filename)
{
    if (!lua_state_ok_)
        return false;

    error_information_ = "";

    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncIsLegalFile());
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
            bool ret = luaToBoolean(lua_state_.getState(), 1, false);
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

void Extension::findFiles(const string& find_with_text, vector<string>& out_files_found)
{
    out_files_found.clear();
    
    if (!lua_state_ok_)
        return;
    
    error_information_ = "";
    
    luaGetGlobal(lua_state_.getState(), LunarGlobal::getInstance().getExtensionFuncFindFiles());
    luaPushString(lua_state_.getState(), find_with_text);
    
    int err = luaCallFunc(lua_state_.getState(), 1, 1);
    if (0 != err)
    {
        error_information_ = strFormat("Extension: %s", luaGetError(lua_state_.getState(), err).c_str());
        LunarMsgBox(error_information_);
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());
        if (ret_cnt > 0)
        {
            if (luaGetType(lua_state_.getState(), 1) == LuaTable)
            {   
                vector< pair<any, any> > vec = luaToArray(lua_state_.getState(), 1);
                vector< pair<any, any> >::iterator it;
                for (it=vec.begin(); it != vec.end(); ++it)
                {
                    out_files_found.push_back(it->second.toString());
                }
            }   
        }
    }
    
    luaPop(lua_state_.getState(), -1);
}
