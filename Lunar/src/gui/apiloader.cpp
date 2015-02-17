#include "apiloader.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>
#include "util/file.hpp"
#include "util/string.hpp"
#include "util/regex.hpp"
#include "lunarcommon.h"
#include "docview.h"

namespace gui
{

const std::string ApiLoader::kApisExt = "api";
const std::string ApiLoader::kApisRelativePath = "/apis";
const std::string ApiLoader::kRegexFunction = "function\\s+(\\w+((\\.|:)\\w+)*\\s*\\(.*\\))";

////////////////////////////////////////////////////
// class name : CurTextTmpApiLoader
// description :
// author :
// time : 2012-01-12-08.04
////////////////////////////////////////////////////
ApiLoader::ApiLoader(QsciScintilla* ptext_edit, QsciAPIs* papis, QObject* parent)
    : QObject(parent),
    ptext_edit_(ptext_edit),
    papis_(papis)
{
}

ApiLoader::~ApiLoader()
{
}

void ApiLoader::LoadFileApis()
{
    if(NULL == papis_)
        return;

    std::list<std::string> api_list;
    util::findFilesInDirRecursively(LunarGlobal::get_app_path() + kApisRelativePath, api_list, kApisExt);
    if (api_list.size()>0)
    {
        for (std::list<std::string>::iterator it = api_list.begin(); it != api_list.end(); ++it)
            papis_->load(StdStringToQString(*it));
    }
}

void ApiLoader::Prepare()
{
    if(NULL == papis_)
        return;

    papis_->prepare();
    QString str;
}

void ApiLoader::ClearTmpApis()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<tmp_apis_vec_.size(); i++)
    {
        papis_->remove(StdStringToQString(tmp_apis_vec_.at(i)));
    }
    tmp_apis_vec_.clear();
}

void ApiLoader::AppendTmpApis()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<tmp_apis_vec_.size(); i++)
    {
        papis_->add(StdStringToQString(tmp_apis_vec_.at(i)));
    }
}

void ApiLoader::ParseCurrentTextApis()
{
    if(NULL == papis_)
        return;

    util::Regex function_regex(kRegexFunction);
    std::string text = "";
    for (int i=0; i<ptext_edit_->lines(); i++)
    {
        text = QStringToStdString(ptext_edit_->text(i));

        if (function_regex.search(text))
        {
            if (function_regex.getMatchedGroupCnt()>1)
            {
                std::string api = function_regex.getMatchedGroup(1);
                api = util::strReplace(api, ":", ".");

                //remove space between function name and "()"
                std::vector<std::string> vec;
                util::strSplit(api, "(", vec);
                if(vec.size() == 2)
                {
                    api = util::strTrim(vec.at(0)) + "(" + vec.at(1);
                }
                tmp_apis_vec_.push_back(api);
            }
        }
    }
}

void ApiLoader::LoadFileObjApis()
{
    if(NULL == papis_)
        return;

    ClassInfo* pclass_info = NULL;
    if(class_map_.find("File") != class_map_.end())
    {
        pclass_info = class_map_["File"];
        delete pclass_info;
    }

    pclass_info = new ClassInfo("File");
    pclass_info->AddClassApi("read(a, b)");
    pclass_info->AddClassApi("close()");
    pclass_info->InsertTmpObj("shit");
    pclass_info->InsertTmpObj("mabi");
    class_map_["File"] = pclass_info;
}

void ApiLoader::ParseCurrentTextObjApis()
{
    if (NULL == papis_)
        return;

    //travers classes
    std::map<std::string, ClassInfo*>::iterator pos;
    for (pos = class_map_.begin(); pos != class_map_.end(); ++pos)
    {
        ClassInfo* pclass_info = pos->second;
        if (NULL != pclass_info)
        {
            //travers tmp objs of one class
            std::set<std::string>::iterator it_tmp_obj;
            for (it_tmp_obj = pclass_info->tmp_obj_name_set_.begin();
                it_tmp_obj != pclass_info->tmp_obj_name_set_.end();
                 ++it_tmp_obj)
            {
                //add all class methods to a obj
                for (int i=0; i<pclass_info->GetClassApiCount(); i++)
                {
                    tmp_apis_vec_.push_back(*it_tmp_obj + "." + pclass_info->GetClassApi(i));
                    //util::PrintLine(*it_tmp_obj + "." + pclass_info->GetClassApi(i));
                }
            }
        }
    }
}

} // namespace gui

