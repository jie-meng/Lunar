#include "apiloader.h"
#include <iterator>
#include <algorithm>
#include <QMessageBox>
#include <QMutexLocker>
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

ApiLoadThread::ApiLoadThread(ApiLoader* papi_loader, QObject *parent) :
    QThread(parent),
    api_dirs_(""),
    parse_supplement_api_script_(""),
    parse_supplement_api_func_(""),
    cursor_line_(0),
    papi_loader_(papi_loader),
    load_api_type_(Unknown),
    loading_(false)
{
    connect(this, SIGNAL(loadFinish(bool, const QString&)), this, SLOT(onLoadFinish(bool, const QString&)));
}

ApiLoadThread::~ApiLoadThread()
{

}

void ApiLoadThread::startLoadCommonApi(const std::string& api_dirs)
{
    if (isRunning() || loading_)
        return;

    if (papi_loader_)
    {
        load_api_type_ = CommonApi;
        api_dirs_ = api_dirs;

        QThread::start();
    }
}

void ApiLoadThread::startRefreshSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func, int cursor_line)
{
    if (isRunning() || loading_)
        return;

    if (papi_loader_)
    {
        load_api_type_ = SupplementApi;
        parse_supplement_api_script_ = parse_supplement_api_script;
        parse_supplement_api_func_ = parse_supplement_api_func;
        cursor_line_ = cursor_line;

        QThread::start();
    }
}

void ApiLoadThread::run()
{
    QMutexLocker mutex_locker(&mutex_);

    if (papi_loader_)
    {
        if (CommonApi == load_api_type_)
        {
            loading_ = true;

            papi_loader_->loadCommonApi(api_dirs_);

            api_dirs_ = "";
            load_api_type_ = Unknown;

            Q_EMIT loadFinish(true, tr(""));
        }
        else if (SupplementApi == load_api_type_)
        {
            loading_ = true;

            std::pair<bool, string> ret = papi_loader_->refreshSupplementApi(parse_supplement_api_script_, parse_supplement_api_func_, cursor_line_);
            parse_supplement_api_script_ = "";
            parse_supplement_api_func_ = "";
            cursor_line_ = 0;
            load_api_type_ = Unknown;

            Q_EMIT loadFinish(ret.first, StdStringToQString(ret.second));
        }
    }
}

void ApiLoadThread::onLoadFinish(bool result, const QString& error_info)
{
    if (result)
    {
        if (papi_loader_)
            papi_loader_->prepare();
    }
    else
    {
        LunarMsgBoxQ(error_info);
    }

    loading_ = false;
}

const std::string kApisExt = "api";

//ApiLoader
ApiLoader::ApiLoader(QsciAPIsEx* papis,
                     const std::string& file) :
    api_load_thread_(this),
    papis_(papis),
	file_(file),
    lua_state_ok_(false),
    error_information_("")
{
    if (NULL == papis_)
        throw new Exception("ApiLoader: QsciAPIsEx* papis is NULL");
}

ApiLoader::~ApiLoader()
{

}

bool ApiLoader::initLuaState(const std::string& parse_supplement_api_script)
{
    if (lua_state_ok_)
        return true;

    if (!isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + parse_supplement_api_script))
    {
        error_information_ =
            strFormat("ApiLoader.initLuaState: extension file %s not exist",
                      (LunarGlobal::getInstance().getAppPath() + "/" + parse_supplement_api_script).c_str());
        lua_state_ok_ = false;
        return false;
    }

    openUtilExtendLibs(lua_state_.getState());

    int err = lua_state_.parseFile(LunarGlobal::getInstance().getAppPath() + "/" + parse_supplement_api_script);
    if (0 != err)
    {
        error_information_ = strFormat("ApiLoader.initLuaState: %s", luaGetError(lua_state_.getState(), err).c_str());
        lua_state_ok_ = false;
    }
    else
    {

        lua_state_.registerFunction("messageBox", scriptMessage);
        lua_state_.registerFunction("sendLog", sendLog);
        error_information_ = "";
        lua_state_ok_ = true;
    }

    return lua_state_ok_;
}

void ApiLoader::loadCommonApi(const std::string& api_paths)
{
    api_files_.clear();

    if (api_paths.length() == 0)
        return;

    vector<string> paths;
    strSplitEx(api_paths, ",", "\"", "\"", paths);
    vector<string>::iterator it;
    for (it = paths.begin(); it != paths.end(); ++it)
    {
        std::string path = strTrim(*it);

        if (isPathDir(splitPathname(file_).first + "/" + path))
        {
            vector<string> api_vec;
            findFilesInDirRecursively(splitPathname(file_).first + "/" + path,
                                      api_vec,
                                      kApisExt);
            if (api_vec.size()>0)
            {
                for (vector<string>::iterator it1 = api_vec.begin(); it1 != api_vec.end(); ++it1)
                    api_files_.push_back(*it1);
            }
        }
        else if (isPathFile(splitPathname(file_).first + "/" + path))
        {
            api_files_.push_back(splitPathname(file_).first + "/" + path);
        }
        else if (isPathDir(currentPath() + "/" + path))
        {
            vector<string> api_vec;
            findFilesInDirRecursively(currentPath() + "/" + path,
                                      api_vec,
                                      kApisExt);
            if (api_vec.size()>0)
            {
                for (vector<string>::iterator it1 = api_vec.begin(); it1 != api_vec.end(); ++it1)
                    api_files_.push_back(*it1);
            }
        }
        else if (isPathFile(currentPath() + "/" + path))
        {
            api_files_.push_back(currentPath() + "/" + path);
        }
        else if (isPathDir(LunarGlobal::getInstance().getAppPath() + "/" + path))
        {
            vector<string> api_vec;
            findFilesInDirRecursively(LunarGlobal::getInstance().getAppPath() + "/" + path,
                                      api_vec,
                                      kApisExt);
            if (api_vec.size()>0)
            {
                for (vector<string>::iterator it1 = api_vec.begin(); it1 != api_vec.end(); ++it1)
                    api_files_.push_back(*it1);
            }
        }
        else if (isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + path))
        {
            api_files_.push_back(LunarGlobal::getInstance().getAppPath() + "/" + path);
        }
    }
}

void ApiLoader::loadCommonApiAsync(const std::string& api_dirs)
{
    api_load_thread_.startLoadCommonApi(api_dirs);
}

void ApiLoader::loadSupplementApiAsync(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func, int cursor_line)
{
    api_load_thread_.startRefreshSupplementApi(parse_supplement_api_script, parse_supplement_api_func, cursor_line);
}

std::pair<bool, std::string> ApiLoader::refreshSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func, int cursor_line)
{
    clearSupplementApi();

    if ("" != parse_supplement_api_script && "" != parse_supplement_api_func)
    {
        bool ret = appendSupplementApi(parse_supplement_api_script, parse_supplement_api_func, cursor_line);
        if (ret)
        {
            set_difference(api_supplement_last_.begin(), api_supplement_last_.end(), api_supplement_.begin(), api_supplement_.end(), back_inserter(remove_apis_));
            set_difference(api_supplement_.begin(), api_supplement_.end(), api_supplement_last_.begin(), api_supplement_last_.end(), back_inserter(append_apis_));
            return std::make_pair(true, "");
        }
        else
        {
            //fail then only clear old supplement apis and return failed
            std::set<std::string>::iterator set_it;
            for (set_it = api_supplement_last_.begin(); set_it != api_supplement_last_.end(); ++set_it)
                remove_apis_.push_back(*set_it);

            return std::make_pair(false, errorInformation());
        }
    }
    else
    {
       //success if not supplement script or func, just return ok
       return std::make_pair(true, "");
    }
}

bool ApiLoader::appendSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func, int cursor_line)
{
    if (!initLuaState(parse_supplement_api_script))
        return false;

    return parseSupplementApi(parse_supplement_api_func, cursor_line);
}

void ApiLoader::clearSupplementApi()
{
    if (!api_files_.empty())
        api_files_.clear();

    remove_apis_.clear();
    append_apis_.clear();
    api_supplement_last_ = api_supplement_;
    api_supplement_.clear();
}

bool ApiLoader::parseSupplementApi(const std::string& parse_supplement_api_func, int cursor_line)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), parse_supplement_api_func);
    luaPushString(lua_state_.getState(), file_);
    luaPushInteger(lua_state_.getState(), cursor_line);

    int err = luaCallFunc(lua_state_.getState(), 2, 1);
    if (0 != err)
    {
        error_information_ = strFormat("ApiLoader.parseSupplementApi: %s", luaGetError(lua_state_.getState(), err).c_str());
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
                api_supplement_.insert(it->second.toString());
        }

        error_information_ = "";
        luaPop(lua_state_.getState(), -1);

        return true;
    }
}

void ApiLoader::prepare()
{  
    vector<string>::iterator it;
    bool change = false;

    for (it = api_files_.begin(); it != api_files_.end(); ++it)
    {
        LogSocket::getInstance().sendLog(strFormat("load api file: %s", (*it).c_str()), "127.0.0.1", LunarGlobal::getInstance().getLogSockPort());
        papis_->load(StdStringToQString(*it));
        change = true;
    }
    api_files_.clear();

    for (it = remove_apis_.begin(); it != remove_apis_.end(); ++it)
    {
        LogSocket::getInstance().sendLog(strFormat("remove api: %s", (*it).c_str()), "127.0.0.1", LunarGlobal::getInstance().getLogSockPort());
        papis_->remove(StdStringToQString(*it));
        change = true;
    }

    for (it = append_apis_.begin(); it != append_apis_.end(); ++it)
    {
        LogSocket::getInstance().sendLog(strFormat("add api: %s", (*it).c_str()), "127.0.0.1", LunarGlobal::getInstance().getLogSockPort());
        papis_->add(StdStringToQString(*it));
        change = true;
    }

    if (change)
        papis_->prepare();
}

} // namespace gui
