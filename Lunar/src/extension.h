#include "util/base.hpp"

class Extension
{
public:
    Extension();
    ~Extension();
    std::string parse(const std::string& extension_file);
private:
    DISALLOW_COPY_AND_ASSIGN(Extension)
};
