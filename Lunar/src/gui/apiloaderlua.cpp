#include "apiloaderlua.h"
#include <list>
#include <QMessageBox>
#include <Qsci/qsciscintilla.h>
#include "qsciapisex.h"
#include "util/file.hpp"
#include "util/regex.hpp"
#include "lunarcommon.h"
#include "docview.h"

using namespace std;
using namespace util;

namespace gui {

const string kRegexFunctionLua = "function\\s+(?<api>(\\w+((\\.|:)\\w+)*\\s*\\(.*\\)))";
const string kRegexIncludeLua = "include\\(\"(?<path>.+)\"\\)";

ApiLoaderLua::ApiLoaderLua(const std::string& file, QsciAPIsEx* papis, QObject* parent) :
    ApiLoader(file, papis, parent)
{
}

void ApiLoaderLua::ParseCurrentFileApi()
{
    ParseFileApi(File(), splitPathname(File()).first);
}

void ApiLoaderLua::ParseIncludeFileApi()
{

}

void ApiLoaderLua::ParseFileApi(const std::string& file, const std::string& dir)
{
    Regex function_regex(kRegexFunctionLua);
    Regex include_regex(kRegexIncludeLua);

    string text = readTextFile(file);
    vector<string> lines;
    text = strReplaceAll(text, "\r\n", "\n");
    strSplit(text, "\n", lines);

    for (int i=0; i<lines.size(); i++)
    {
        if (function_regex.match(strTrim(lines[i])))
        {
            string api = function_regex.getMatchedGroup("api");
            if (api != "")
                AddApiCurrentFile(strReplace(api, ":", "."));
        }
        else if (include_regex.match(strTrim(lines[i])))
        {
            string file = dir + "/" + include_regex.getMatchedGroup("path");
            if (!isPathFile(file))
                continue;

            ParseFileApi(file, dir);
        }
    }
}

}

