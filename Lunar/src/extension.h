#include "util/luax.hpp"

class Extension
{
public:
    SINGLETON(Extension)
    Extension();
    ~Extension();
    bool init();
    inline std::string errorInfo() const { return error_information_; }
    inline bool isOk() const { return lua_state_ok_; }
    bool parseFilename(const std::string& filename,
               std::string* pout_type,
               size_t* pauto_complete_type_,
               std::string* pout_api,
               std::string* pout_executor,
               std::string* pout_parse_supplement_api_script = NULL,
               std::string* pout_parse_supplement_api_func = NULL);
    std::string fileFilter();
    bool ignoreFile(const std::string& filename);
private:
    bool initLuaState();
private:
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
private:
    DISALLOW_COPY_AND_ASSIGN(Extension)
};
