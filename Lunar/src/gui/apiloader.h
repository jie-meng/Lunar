#ifndef GUI_APILOADER_H
#define GUI_APILOADER_H

#include <vector>
#include <map>
#include <set>
#include <QtCore/QObject>
#include "util/base.hpp"

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

} // namespace gui



#endif // GUI_TMPAPILOADER_H
