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

//ApiLoaderEx
ApiLoaderEx::ApiLoaderEx(const std::string& file,
                         QsciAPIsEx* papis) :
    file_(file),
    papis_(papis),
    lua_state_ok_(false),
    error_information_("")
{
    if (NULL == papis_)
        throw new Exception("ApiLoaderEx: QsciAPIsEx* papis is NULL");
}

ApiLoaderEx::~ApiLoaderEx()
{

}

bool ApiLoaderEx::initLuaState(const std::string& parse_supplement_api_script)
{
    if (!isPathFile(LunarGlobal::getInstance().getPluginsDir() + "/" + parse_supplement_api_script))
    {
        error_information_ =
            strFormat("ApiLoaderEx.initLuaState: extension file %s not exist",
                      (LunarGlobal::getInstance().getPluginsDir() + "/" + parse_supplement_api_script).c_str());
        lua_state_ok_ = false;
        return false;
    }

    openUtilExtendLibs(lua_state_.getState());

    int err = lua_state_.parseFile(LunarGlobal::getInstance().getPluginsDir() + "/" + parse_supplement_api_script);
    if (0 != err)
    {
        error_information_ = strFormat("ApiLoaderEx.initLuaState: %s", luaGetError(lua_state_.getState(), err).c_str());
        lua_state_ok_ = false;
    }
    else
    {
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

void ApiLoaderEx::loadApi(const std::string& api_dirs)
{
    vector<string> dirs;
    strSplitEx(api_dirs, ",", "\"", "\"", dirs);
    vector<string>::iterator it;
    for (it = dirs.begin(); it != dirs.end(); ++it)
    {
        vector<string> api_vec;
        findFilesInDirRecursively(*it, api_vec, kApisExt);
        if (api_vec.size()>0)
        {
            for (vector<string>::iterator it1 = api_vec.begin(); it1 != api_vec.end(); ++it1)
                papis_->load(StdStringToQString(*it1));
        }
    }
}

void ApiLoaderEx::appendSupplementApi(const std::string& parse_supplement_api_script)
{
    if (!parseSupplementApi(parse_supplement_api_script))
        return;

    for (uint32_t i=0; i<api_supplement_.size(); i++)
        papis_->add(StdStringToQString(api_supplement_[i]));
}

void ApiLoaderEx::clearSupplementApi()
{
    for (uint32_t i=0; i<api_supplement_.size(); i++)
        papis_->remove(StdStringToQString(api_supplement_[i]));
    api_supplement_.clear();
}

bool ApiLoaderEx::parseSupplementApi(const std::string& parse_supplement_api_script)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), "parseSupplementApi");
    luaPushString(lua_state_.getState(), file_);

    int err = luaCallFunc(lua_state_.getState(), 1, 1);
    if (0 != err)
    {
        error_information_ = strFormat("ApiLoaderEx.parseSupplementApi: %s", luaGetError(lua_state_.getState(), err).c_str());
        luaPop(lua_state_.getState(), -1);

        return false;
    }
    else
    {
        int ret_cnt = luaGetTop(lua_state_.getState());

        vector< pair<any, any> > vec;
        if (ret_cnt > 0)
            vec = luaGetTable(lua_state_.getState(), 1);

        if (!vec.empty())
        {
            vector< pair<any, any> >::iterator it;
            for (it = vec.begin(); it != vec.end(); ++it)
                api_supplement_.push_back(it->second.toString());
        }

        error_information_ = "";
        luaPop(lua_state_.getState(), -1);

        return true;
    }
}

void ApiLoaderEx::prepare()
{
    papis_->prepare();
}

} // namespace gui
