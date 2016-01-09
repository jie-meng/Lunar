#include "goto_manager.h"
#include "util/file.hpp"
#include "util/luaextend.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

GotoManager::GotoManager()
{
    openUtilExtendLibs(lua_state_.getState());
    lua_state_.registerFunction("messageBox", scriptMessage);
    lua_state_.registerFunction("sendLog", sendLog);
}

bool GotoManager::getDefinitions(
        const std::string& script,
        const std::string& func,
        const std::string& project_dir,
        const std::string& current_file,
        const std::string& text,
        std::vector< std::string >& out_result)
{
    if (!isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + script))
    {
        LunarMsgBox(strFormat("Error<GotoManager::gotoDefinition> Script %s does not exist.", script.c_str()));
        return false;
    }

    int err = lua_state_.parseFile(LunarGlobal::getInstance().getAppPath() + "/" + script);
    if (0 != err)
    {
        LunarMsgBox(strFormat("Error<GotoManager::gotoDefinition> Parse script failed: %s.", luaGetError(lua_state_.getState(), err).c_str()));
        return false;
    }
    else
    {
        luaGetGlobal(lua_state_.getState(), func);
        luaPushString(lua_state_.getState(), project_dir);
        luaPushString(lua_state_.getState(), current_file);
        luaPushString(lua_state_.getState(), text);

        int err = luaCallFunc(lua_state_.getState(), 3, 1);
        if (0 != err)
        {
            LunarMsgBox(strFormat("Error<GotoManager::gotoDefinition> Execute func failed: %s.", luaGetError(lua_state_.getState(), err).c_str()));
            luaPop(lua_state_.getState(), -1);

            return false;
        }
        else
        {
            int ret_cnt = luaGetTop(lua_state_.getState());

            vector< pair<any, any> > vec;
            if (ret_cnt > 0)
                vec = luaGetTable(lua_state_.getState(), 1);

            if (!vec.empty())
            {
                vector< pair<any, any> >::iterator it;
                for (it = vec.begin(); it != vec.end(); ++it)
                    out_result.push_back(it->second.toString());
            }

            luaPop(lua_state_.getState(), -1);

            return true;
        }
    }
}

