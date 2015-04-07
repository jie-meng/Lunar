#ifndef GUI_APILOADER_H
#define GUI_APILOADER_H

#include <vector>
#include <map>
#include <set>
#include <QtCore/QObject>
#include "util/base.hpp"
#include "util/luaextend.hpp"

namespace gui
{

class QsciAPIsEx;
class ClassInfo;

////////////////////////////////////////////////////
// class name : ApiLoader
// description :
// author :
// time : 2012-01-12-08.01
////////////////////////////////////////////////////
class ApiLoader : public QObject
{
    Q_OBJECT
public:
    explicit ApiLoader(const std::string& file, QsciAPIsEx* papis, QObject* parent);
    virtual ~ApiLoader();

    void loadFileApis(const std::string& api_path);
    void prepare();

    void clearApiCurrentFile();
    void appendApiCurrentFile();
    void clearApiIncludeFile();
    void appendApiIncludeFile();
    virtual void parseCurrentFileApi();
    virtual void parseIncludeFileApi();
protected:
    void addApiCurrentFile(const std::string& str);
    void addApiIncludeFile(const std::string& str);
    inline std::string file() const { return file_; }
private:
    QsciAPIsEx* papis_;
    std::string file_;
    std::vector<std::string> current_file_apis_vec_;
    std::vector<std::string> include_file_apis_vec_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoader)
};

class ApiLoaderEx
{
public:
    ApiLoaderEx(const std::string& file,
                QsciAPIsEx* papis);
    ~ApiLoaderEx();

    bool initLuaState(const std::string& parse_supplement_api_script);
    void loadApi(const std::string& api_dirs);
    void appendSupplementApi(const std::string& parse_supplement_api_script);
    void clearSupplementApi();
    void prepare();
private:
    bool parseSupplementApi(const std::string& parse_supplement_api_script);
private:
    QsciAPIsEx* papis_;
    std::string file_;
    std::vector<std::string> api_supplement_;
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoaderEx)
};

} // namespace gui



#endif // GUI_TMPAPILOADER_H
