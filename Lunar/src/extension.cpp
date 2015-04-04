#include "extension.h"
#include <vector>
#include "util/file.hpp"
#include "util/regex.hpp"

using namespace std;
using namespace util;

Extension::Extension()
{
}

Extension::~Extension()
{
}

std::string parse(const std::string& extension_file)
{
   string text = readTextFile(extension_file); 
   vector<string> lines;
   strSplit(text, "\n", lines);
   vector<string>::iterator it = lines.begin();
   for (;it != lines.end(); ++it)
   {
       Regex regex("(?<host>\\w+)\\.(?<action>\\w+)\\.(?<c>\\w+)\\s*=\\s*(?<obj>\\w+)");
   }
}
