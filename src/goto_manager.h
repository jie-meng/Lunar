#ifndef GOTOMANAGER_H
#define GOTOMANAGER_H

#include <vector>
#include "util/luax.hpp"

class GotoManager
{
public:
    SINGLETON(GotoManager)
    GotoManager();
    bool getDefinitions(
            const std::string& script,
            const std::string& func,
            const std::string& project_dir,
            const std::string& current_file,
            const std::string& text,
            std::vector< std::string >& out_result);
private:
    util::LuaState lua_state_;
private:
    DISALLOW_COPY_AND_ASSIGN(GotoManager)
};

#endif // GOTOMANAGER_H
