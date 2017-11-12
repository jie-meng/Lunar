#ifndef EXTENSION_H
#define EXTENSION_H

#include "util/luax.hpp"
#include <vector>
#include <map>

class Extension
{
public:
    SINGLETON(Extension)
    Extension();
    ~Extension();
    bool init();
    inline std::string errorInfo() const { return error_information_; }
    inline bool isOk() const { return lua_state_ok_; }
    bool parseFilename(const std::string& filename, std::map<std::string, std::string>& out_map);
    std::string fileFilter();
    bool isLegalFile(const std::string& filename);
    void findFiles(const std::string& find_with_text, std::vector<std::string>& out_files_found);
private:
    bool initLuaState();
private:
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
private:
    DISALLOW_COPY_AND_ASSIGN(Extension)
};

#endif // EXTENSION_H
