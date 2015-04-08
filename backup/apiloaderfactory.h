#ifndef GUI_APILOADERFACTORY_H
#define GUI_APILOADERFACTORY_H

#include "util/base.hpp"
#include "filetype.h"
#include "apiloader.h"

namespace gui
{

class ApiLoaderFactory
{
public:
    SINGLETON(ApiLoaderFactory)
    ApiLoader* createApiLoader(FileType file_type,
                               const std::string& file,
                               QsciAPIsEx* papis,
                               QObject* parent);
private:
    ApiLoaderFactory();
    ~ApiLoaderFactory();
private:
    DISALLOW_COPY_AND_ASSIGN(ApiLoaderFactory)
};

}

#endif // GUI_APILOADERFACTORY_H
