#ifndef APILOADERLUA_H
#define APILOADERLUA_H

#include "apiloader.h"

namespace gui
{

class ApiLoaderLua : public ApiLoader
{
    Q_OBJECT
public:
    explicit ApiLoaderLua(const std::string& file, QsciAPIsEx* papis, QObject* parent);
    virtual void ParseCurrentFileApi();
    virtual void ParseIncludeFileApi();
signals:

public slots:

private:
    void ParseFileApi(const std::string& file, const std::string& dir);
};

}

#endif // APILOADERLUA_H
