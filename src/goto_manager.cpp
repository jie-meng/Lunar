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
        const std::string& text,
        int line,
        const std::string& filename,
        const std::string& project_dir,
        std::vector< std::string >& out_result)
{
    if (!isPathFile(getExtensionAbsolutePath(script)))
    {
        LunarMsgBox(strFormat("Error<GotoManager::gotoDefinition> Script %s does not exist.", getExtensionAbsolutePath(script).c_str()));
        return false;
    }

    int err = lua_state_.parseFile(getExtensionAbsolutePath(script));
    if (0 != err)
    {
        LunarMsgBox(strFormat("Error<GotoManager::gotoDefinition> Parse script failed: %s.", luaGetError(lua_state_.getState(), err).c_str()));
        return false;
    }
    else
    {
        luaGetGlobal(lua_state_.getState(), func);

        luaPushString(lua_state_.getState(), text);
        luaPushInteger(lua_state_.getState(), line);
        luaPushString(lua_state_.getState(), filename);
        luaPushString(lua_state_.getState(), project_dir);

        int err = luaCallFunc(lua_state_.getState(), 4, 1);
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
                vec = luaToArray(lua_state_.getState(), 1);

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

