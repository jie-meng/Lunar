#ifndef APILOADEROCTAVE_H
#define APILOADEROCTAVE_H

#include "apiloader.h"

namespace gui
{

class ApiLoaderOctave : public ApiLoader
{
    Q_OBJECT
public:
    explicit ApiLoaderOctave(const std::string& file, QsciAPIsEx* papis, QObject* parent);
    virtual void ParseCurrentFileApi();
    virtual void ParseIncludeFileApi();
signals:

public slots:

private:
};

}

#endif // APILOADEROCTAVE_H
