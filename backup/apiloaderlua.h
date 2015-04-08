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
    virtual void parseCurrentFileApi();
    virtual void parseIncludeFileApi();
signals:

public slots:

private:
    void parseFileApiRecursively(const std::string& file, const std::string& dir);
};

}

#endif // APILOADERLUA_H
