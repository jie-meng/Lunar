#include "apiloader.h"
#include <QMessageBox>
#include <Qsci/qsciscintilla.h>
#include "qsciapisex.h"
#include "util/file.hpp"
#include "util/regex.hpp"
#include "lunarcommon.h"
#include "docview.h"

using namespace std;
using namespace util;

namespace gui
{

const std::string kApisExt = "api";
//const std::string ApiLoader::kRegexFunction = "function\\s+(\\w+((\\.|:)\\w+)*\\s*\\(.*\\))";

////////////////////////////////////////////////////
// class name : ApiLoader
// description :
// author :
// time : 2012-01-12-08.04
////////////////////////////////////////////////////
ApiLoader::ApiLoader(const std::string& file, QsciAPIsEx* papis, QObject* parent)
    : QObject(parent),
    file_(file),
    papis_(papis)
{
}

ApiLoader::~ApiLoader()
{
}

void ApiLoader::loadFileApis(const std::string& api_path)
{
    if(NULL == papis_)
        return;

    std::list<std::string> api_list;
    util::findFilesInDirRecursively(api_path, api_list, kApisExt);
    if (api_list.size()>0)
    {
        for (std::list<std::string>::iterator it = api_list.begin(); it != api_list.end(); ++it)
            papis_->load(StdStringToQString(*it));
    }
}

void ApiLoader::prepare()
{
    if(NULL == papis_)
        return;

    papis_->prepare();
}

void ApiLoader::clearApiCurrentFile()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<current_file_apis_vec_.size(); i++)
        papis_->remove(StdStringToQString(current_file_apis_vec_.at(i)));
    current_file_apis_vec_.clear();
}

void ApiLoader::appendApiCurrentFile()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<current_file_apis_vec_.size(); i++)
        papis_->add(StdStringToQString(current_file_apis_vec_.at(i)));
}

void ApiLoader::clearApiIncludeFile()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<include_file_apis_vec_.size(); i++)
        papis_->remove(StdStringToQString(include_file_apis_vec_.at(i)));
    include_file_apis_vec_.clear();
}

void ApiLoader::appendApiIncludeFile()
{
    if(NULL == papis_)
        return;

    for (uint32_t i=0; i<include_file_apis_vec_.size(); i++)
        papis_->add(StdStringToQString(include_file_apis_vec_.at(i)));
}

void ApiLoader::parseCurrentFileApi()
{
}

void ApiLoader::parseIncludeFileApi()
{
}

void ApiLoader::addApiCurrentFile(const std::string& str)
{
    current_file_apis_vec_.push_back(str);
}

void ApiLoader::addApiIncludeFile(const std::string& str)
{
    include_file_apis_vec_.push_back(str);
}

} // namespace gui
