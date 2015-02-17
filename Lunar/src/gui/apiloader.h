#ifndef GUI_APILOADER_H
#define GUI_APILOADER_H

#include <vector>
#include <map>
#include <set>
#include <QtCore/QObject>
#include "util/base.hpp"

class QsciAPIs;
class QsciScintilla;

namespace gui
{

class ClassInfo;

////////////////////////////////////////////////////
// class name : CurTextApiLoader
// description :
// author :
// time : 2012-01-12-08.01
////////////////////////////////////////////////////
class ApiLoader : public QObject
{
    Q_OBJECT
public:
    explicit ApiLoader(QsciScintilla* ptext_edit, QsciAPIs* papis, QObject* parent);
    virtual ~ApiLoader();

    void LoadFileApis();
    void LoadFileObjApis();
    void Prepare();

    void ClearTmpApis();
    void AppendTmpApis();
    void ParseCurrentTextApis();
    void ParseCurrentTextObjApis();

    static const std::string kApisExt;
    static const std::string kApisRelativePath;
    static const std::string kRegexFunction;

private:
    QsciScintilla* ptext_edit_;
    QsciAPIs* papis_;
    std::vector<std::string> tmp_apis_vec_;
    std::map<std::string, ClassInfo*> class_map_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoader)
};

////////////////////////////////////////////////////
// class name : ClassInfo
// description :
// author :
// time : 2012-01-12-14.08
////////////////////////////////////////////////////
class ClassInfo
{
    friend class ApiLoader;
public:
    ClassInfo(const std::string& class_name) : class_name_(class_name) {}
    ~ClassInfo() {}
    std::string GetClassName() const { return class_name_; }
    void AddClassApi(const std::string& entry) { class_api_vec_.push_back(entry); }
    void InsertTmpObj(const std::string& obj_name) { tmp_obj_name_set_.insert(obj_name); }
    void ClearTmpObj() { tmp_obj_name_set_.clear(); }

    int GetClassApiCount() { return class_api_vec_.size(); }
    int GetTmpObjCount() { return tmp_obj_name_set_.size(); }
    std::string GetClassApi(int index) { return class_api_vec_.at(index); }
private:
    std::string class_name_;
    std::vector<std::string> class_api_vec_;
    std::set<std::string> tmp_obj_name_set_;
private:
    DISALLOW_COPY_AND_ASSIGN(ClassInfo)
};

} // namespace gui



#endif // GUI_TMPAPILOADER_H
