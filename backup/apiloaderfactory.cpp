#include "apiloaderfactory.h"
#include "apiloaderlua.h"
#include "apiloaderoctave.h"

namespace gui
{

ApiLoaderFactory::ApiLoaderFactory()
{
    //ctor
}

ApiLoaderFactory::~ApiLoaderFactory()
{
    //dtor
}

ApiLoader* ApiLoaderFactory::createApiLoader(FileType file_type,
                                             const std::string& file,
                                             QsciAPIsEx* papis,
                                             QObject* parent)
{
    switch (file_type)
    {
    case Lua:
        return new ApiLoaderLua(file, papis, parent);
    case Octave:
        return new ApiLoaderOctave(file, papis, parent);
    default:
        return new ApiLoader(file, papis, parent);
    }
}

}
