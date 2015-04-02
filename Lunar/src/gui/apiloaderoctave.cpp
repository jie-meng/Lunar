#include "apiloaderoctave.h"
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

namespace gui
{

//const std::string kRegexFunction = "function\\s+(\\w+((\\.|:)\\w+)*\\s*\\(.*\\))";
const string kRegexFunctionOctave = "function\\s+((\\[.*\\]\\s*=)|(\\w+\\s*=))?\\s*(?<api>\\w+\\s*(?<param>\\(.*\\)))";

class OctaveFileFilter : public PathFilter
{
public:
    virtual bool filter(const std::string& path)
    {
        std::vector<std::string> filterVec;
        util::strSplit(LunarGlobal::getOctaveFileFilter(), ",", filterVec);
        for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
        {
            if (util::strEndWith(path, std::string(".") + *it, false))
                return true;
        }

        return false;
    }
};

ApiLoaderOctave::ApiLoaderOctave(const std::string& file, QsciAPIsEx* papis, QObject* parent) :
    ApiLoader(file, papis, parent)
{

}

void ApiLoaderOctave::ParseCurrentFileApi()
{
    Regex function_regex(kRegexFunctionOctave);
    string text = readTextFile(File());
    vector<string> lines;
    text = strReplaceAll(text, "\r\n", "\n");
    strSplit(text, "\n", lines);
    for (int i=0; i<lines.size(); i++)
    {
        if (function_regex.match(strTrim(lines[i])))
        {
            string api = function_regex.getMatchedGroup("api");
            if (api != "")
                AddApiCurrentFile(api);
        }
    }
}

void ApiLoaderOctave::ParseIncludeFileApi()
{
    pair<string, string> path_name = splitPathname(File());
    OctaveFileFilter octave_file_filter;
    list<string> ls;
    listFiles(path_name.first, ls, &octave_file_filter);
    for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it)
    {
        string text = readTextFile(*it);
        vector<string> lines;
        text = strReplaceAll(text, "\r\n", "\n");
        strSplit(text, "\n", lines);
        for (size_t i=0; i<lines.size(); ++i)
        {
            string trim_line = strTrim(lines[i]);
            if (trim_line != "" &&
                !strStartWith(trim_line, "#") &&
                !strStartWith(trim_line, "%"))
            {
                Regex function_regex(kRegexFunctionOctave);
                if (function_regex.match(trim_line))
                {
                    string param = function_regex.getMatchedGroup("param");
                    if (param != "")
                    {
                        //QMessageBox::information(NULL, StdStringToQString(trim_line), StdStringToQString(fileBaseName(path_name.second) + param));
                        AddApiIncludeFile(fileBaseName(splitPathname(*it).second) + param);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
}

}
