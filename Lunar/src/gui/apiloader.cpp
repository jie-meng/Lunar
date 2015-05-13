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

ApiLoadThread::ApiLoadThread(ApiLoader* papi_loader, QObject *parent) :
    QThread(parent),
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

void ApiLoadThread::startRefreshSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func)
{
    if (isRunning() || loading_)
        return;

    if (papi_loader_)
    {
        load_api_type_ = SupplementApi;
        parse_supplement_api_script_ = parse_supplement_api_script;
        parse_supplement_api_func_ = parse_supplement_api_func;

        QThread::start();
    }
}

void ApiLoadThread::run()
{
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

            std::pair<bool, string> ret = papi_loader_->refreshSupplementApi(parse_supplement_api_script_, parse_supplement_api_func_);
            parse_supplement_api_script_ = "";
            parse_supplement_api_func_ = "";
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
                    papis_->load(StdStringToQString(*it1));
            }
        }
        else if (isPathFile(splitPathname(file_).first + "/" + path))
        {
            papis_->load(StdStringToQString(splitPathname(file_).first + "/" + path));
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
                    papis_->load(StdStringToQString(*it1));
            }
        }
        else if (isPathFile(currentPath() + "/" + path))
        {
            papis_->load(StdStringToQString(currentPath() + "/" + path));
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
                    papis_->load(StdStringToQString(*it1));
            }
        }
        else if (isPathFile(LunarGlobal::getInstance().getAppPath() + "/" + path))
        {
            papis_->load(StdStringToQString(LunarGlobal::getInstance().getAppPath() + "/" + path));
        }
    }
}

void ApiLoader::loadCommonApiAsync(const std::string& api_dirs)
{
    api_load_thread_.startLoadCommonApi(api_dirs);
}

void ApiLoader::loadSupplementApiAsync(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func)
{
    api_load_thread_.startRefreshSupplementApi(parse_supplement_api_script, parse_supplement_api_func);
}

std::pair<bool, std::string> ApiLoader::refreshSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func)
{
    clearSupplementApi();
    if ("" != parse_supplement_api_script && "" != parse_supplement_api_func)
    {
        if (!appendSupplementApi(parse_supplement_api_script, parse_supplement_api_func))
            return std::make_pair(false, errorInformation());
    }

    return std::make_pair(true, "");
}

bool ApiLoader::appendSupplementApi(const std::string& parse_supplement_api_script, const std::string& parse_supplement_api_func)
{
    if (!initLuaState(parse_supplement_api_script))
        return false;

    if (!parseSupplementApi(parse_supplement_api_func))
        return false;

    set<string>::iterator it;
    for (it = api_supplement_.begin(); it != api_supplement_.end(); ++it)
        papis_->add(StdStringToQString(*it));

    return true;
}

void ApiLoader::clearSupplementApi()
{
    set<string>::iterator it;
    for (it = api_supplement_.begin(); it != api_supplement_.end(); ++it)
        papis_->remove(StdStringToQString(*it));
    api_supplement_.clear();
}

bool ApiLoader::parseSupplementApi(const std::string& parse_supplement_api_func)
{
    if (!lua_state_ok_)
        return false;

    luaGetGlobal(lua_state_.getState(), parse_supplement_api_func);
    luaPushString(lua_state_.getState(), file_);

    int err = luaCallFunc(lua_state_.getState(), 1, 1);
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
    papis_->prepare();
}

} // namespace gui
