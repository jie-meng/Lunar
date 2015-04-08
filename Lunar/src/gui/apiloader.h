#ifndef GUI_APILOADER_H
#define GUI_APILOADER_H

#include <vector>
#include <set>
#include <QtCore/QObject>
#include "util/base.hpp"
#include "util/luaextend.hpp"

namespace gui
{

class QsciAPIsEx;
class ClassInfo;

class ApiLoader
{
public:
    ApiLoader(const std::string& file,
                QsciAPIsEx* papis);
    ~ApiLoader();

    void loadApi(const std::string& api_dirs);
    bool appendSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func);
    void clearSupplementApi();
    void prepare();
    inline std::string errorInformation() const { return error_information_; }
private:
    bool initLuaState(const std::string& parse_supplement_api_script);
    bool parseSupplementApi(const std::string& parse_supplement_api_func);
private:
    QsciAPIsEx* papis_;
    std::string file_;
    std::set<std::string> api_supplement_;
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoader)
};

} // namespace gui



#endif // GUI_TMPAPILOADER_H
