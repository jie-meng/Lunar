#ifndef GUI_APILOADER_H
#define GUI_APILOADER_H

#include <vector>
#include <set>
#include <QtCore/QObject>
#include <QThread>
#include <QMutex>
#include "util/base.hpp"
#include "util/luaextend.hpp"

namespace gui
{

class ApiLoader;

class ApiLoadThread : public QThread
{
    Q_OBJECT

    enum LoadApiType
    {
        Unknown,
        CommonApi,
        SupplementApi
    };

public:
    ApiLoadThread(ApiLoader* papi_loader, QObject *parent = 0);
    virtual ~ApiLoadThread();
    void startLoadCommonApi(const std::string& api_dirs);
    void startRefreshSupplementApi(const std::string& parse_supplement_api_script,
                                   const std::string& parse_supplement_api_func,
                                   int cursor_line,
                                   const std::string& project_src_dir);
    inline void resetLoading() { loading_ = false; }
signals:
    void loadFinish(bool, const QString&);
protected:
    virtual void run();
public slots:
    void onLoadFinish(bool result, const QString& error_info);
    void onPrepareFinish();
private:
    QMutex mutex_;
    std::string api_dirs_;
    std::string parse_supplement_api_script_;
    std::string parse_supplement_api_func_;
    int cursor_line_;
    std::string project_src_dir_;
    ApiLoader* papi_loader_;
    LoadApiType load_api_type_;
    bool loading_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoadThread)
};

class QsciAPIsEx;
class ClassInfo;

class ApiLoader
{
public:
    friend class ApiLoadThread;

    ApiLoader(QsciAPIsEx* papis,
              const std::string& file);
    ~ApiLoader();

    void loadCommonApiAsync(const std::string& api_dirs);
    void loadSupplementApiAsync(const std::string& parse_supplement_api_script,
                                const std::string& parse_supplement_api_func,
                                int cursor_line,
                                const std::string& project_src_dir);
    inline std::string errorInformation() const { return error_information_; }
    inline QsciAPIsEx* getApis() { return papis_; }
private:
    bool initLuaState(const std::string& parse_supplement_api_script);
    void loadCommonApi(const std::string& api_dirs);
    std::pair<bool, std::string> refreshSupplementApi(const std::string& parse_supplement_api_script,
                                                      const std::string& parse_supplement_api_func,
                                                      int cursor_line,
                                                      const std::string& project_src_dir);
    bool parseSupplementApi(const std::string& parse_supplement_api_func,
                            int cursor_line,
                            const std::string& project_src_dir);
    bool appendSupplementApi(const std::string& parse_supplement_api_script,
                             const std::string& parse_supplement_api_func,
                             int cursor_line,
                             const std::string& project_src_dir);
    void clearSupplementApi();
    void prepare();
private:
    QsciAPIsEx* papis_;
    ApiLoadThread api_load_thread_;
    std::string file_;
    std::vector<std::string> api_files_;
    std::set<std::string> api_supplement_last_;
    std::set<std::string> api_supplement_;
    std::vector<std::string> remove_apis_;
    std::vector<std::string> append_apis_;
    util::LuaState lua_state_;
    bool lua_state_ok_;
    std::string error_information_;
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoader)
};

} // namespace gui



#endif // GUI_TMPAPILOADER_H
