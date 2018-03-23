#include "docview.h"
#include <vector>
#include <fstream>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <Qsci/qsciabstractapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexeravs.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercoffeescript.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexeridl.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerjson.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexermarkdown.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpo.h>
#include <Qsci/qscilexerpostscript.h>
#include <Qsci/qscilexerpov.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexerspice.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexertcl.h>
#include <Qsci/qscilexertex.h>
#include <Qsci/qscilexerverilog.h>
#include <Qsci/qscilexervhdl.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexeryaml.h>
#include "lexernone.h"
#include "util/file.hpp"
#include "util/regex.hpp"
#include "util/cfg.hpp"
#include "util/json.hpp"
#include "lunarapi.h"
#include "lunarcommon.h"
#include "extension.h"
#include "mainwindow.h"
#include "apiloader.h"
#include "goto_manager.h"

using namespace std;
using namespace util;

namespace gui
{

int DocView::s_new_docview_sequence_ = 1;

DocView::DocView(const QString& pathname, QWidget* parent)
    : QWidget(parent),
    save_dialog_init_dir_("."),
    pathname_(tr("")),
    papis_(NULL),
    plexer_(NULL),
    is_apis_preparing_(false),
    papi_loader_(NULL),
    file_type_(Unknown),
    executor_(""),
    parse_api_script_(""),
    project_src_dir_(""),
    goto_script_(""),
    selection_match_indicator_(0),
    new_file_sequence_no_(0)
{
    ptext_edit_ = new QsciScintilla(parent);
    setPathname(pathname);

    init();
}

DocView::~DocView()
{
    clearLexerApi();
}

QString DocView::getTitle()
{
    auto title = getTitleFromPath(getPathname());
    if(tr("") == title)
    {
        if (new_file_sequence_no_ == 0)
            new_file_sequence_no_ = DocView::s_new_docview_sequence_++;

        return StdStringToQString(strFormat("New_%d", new_file_sequence_no_));
    }
    else
    {
        return ptext_edit_->isModified() ? "*" + title : title;
    }
}

int DocView::getCurrentLine()
{
    int line;
    int index;
    ptext_edit_->getCursorPosition(&line, &index);
    return line + 1;
}

void DocView::focusOnText()
{
    ptext_edit_->setFocus();
}

void DocView::clearLexerApi()
{
    safeDelete(plexer_);
    //papis_ would be delete together with plexer_

    safeDelete(papi_loader_);

    executor_ = "";
    parse_api_script_ = "";
    file_type_ = Unknown;
}

void DocView::setLexerApi()
{
    clearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::getInstance().getAutocompletionThreshold());
    ptext_edit_->setFont(LunarGlobal::getInstance().getFont());
    ptext_edit_->setAutoCompletionCaseSensitivity(false);
    if(LunarGlobal::getInstance().getAutocompletionWordtip())
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    else
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    //Extension
    if (Extension::getInstance().isOk())
    {
        std::string filename = QStringToStdString(pathname_);
        map<string, string> dict;
        if (Extension::getInstance().parseFilename(filename, dict))
        {
            FileType filetype = Unknown;
            string type = getValueFromMap<string>(dict, "type", "");
            plexer_ = getLexerFromTypeName(type, &filetype);

            size_t auto_complete_type_ = getValueFromMap<size_t>(dict, "auto_complete_type", 0);
            executor_ = getValueFromMap<string>(dict, "executor", "");
            string apis = getValueFromMap<string>(dict, "api", "");
            string templates = getValueFromMap<string>(dict, "templates", "");
            parse_api_script_ = getValueFromMap<string>(dict, "plugin_parse_api", "");
            goto_script_ = getValueFromMap<string>(dict, "plugin_goto", "");
            project_src_dir_ = getValueFromMap<string>(dict, "project_src_dir", "");
            comment_line_symbol_ = StdStringToQString(getValueFromMap<string>(dict, "comment_line", ""));
            comment_block_symbol_begin_ = StdStringToQString(getValueFromMap<string>(dict, "comment_block_begin", ""));
            comment_block_symbol_end_ = StdStringToQString(getValueFromMap<string>(dict, "comment_block_end", ""));

            string override_file = currentPath() + "/luna_ext_" + type + ".json";
            if (isPathFile(override_file))
            {
                try
                {
                    auto override = util::Json::parse(readTextFile(override_file));
                    if (!override.get("auto_complete_type").isNull())
                        auto_complete_type_ = override.get("auto_complete_type").toInt();

                    if (!override.get("executor").isNull())
                        executor_ = override.get("executor").toString();

                    if (!override.get("api").isNull())
                        apis = override.get("api").toString();

                    if (!override.get("templates").isNull())
                        templates = override.get("templates").toString();

                    if (!override.get("plugin_parse_api").isNull())
                        parse_api_script_ = override.get("plugin_parse_api").toString();

                    if (!override.get("plugin_goto").isNull())
                        goto_script_ = override.get("plugin_goto").toString();

                    if (!override.get("project_src_dir").isNull())
                        project_src_dir_ = override.get("project_src_dir").toString();

                    if (!override.get("comment_line").isNull())
                        comment_line_symbol_ = StdStringToQString(override.get("comment_line").toString());

                    if (!override.get("comment_block_begin").isNull())
                        comment_block_symbol_begin_ = StdStringToQString(override.get("comment_block_begin").toString());

                    if (!override.get("comment_block_end").isNull())
                        comment_block_symbol_end_ = StdStringToQString(override.get("comment_block_end").toString());
                }
                catch (std::exception& e)
                {
                    LunarMsgBox(strFormat("Parse %s failed: %s", override_file.c_str(), e.what()));
                }
            }

            ptext_edit_->setLexer(plexer_);
            plexer_->setFont(LunarGlobal::getInstance().getFont());
            if (0 == auto_complete_type_)
                papis_ = new QsciAPIsEx(plexer_);
            else
                papis_ = new LunarApi(plexer_);
            plexer_->setAPIs(papis_);
            ptext_edit_->setLexer(plexer_);

            //templates
            loadTemplates(templates);

            //api
            papi_loader_ = new ApiLoader(papis_, QStringToStdString(pathname_));
            //add templates keys first
            for (auto it = templates_.begin(); it != templates_.end(); ++it)
            {
                papi_loader_->getApis()->add(StdStringToQString(it->first));
            }
            papi_loader_->getApis()->prepare();
            papi_loader_->loadCommonApiAsync(apis);
            //do not load supplement api when first load, because it'll not work until loadCommonApiAsync ended.

            //parse success
            file_type_ = filetype;

            return;
        }
        else
        {
            LunarMsgBox(Extension::getInstance().errorInfo());
        }
    }

    ptext_edit_->setLexer(NULL);
    ptext_edit_->setAutoCompletionCaseSensitivity(false);
    file_type_ = Unknown;
}

void DocView::loadTemplates(const std::string& template_str)
{
    if (template_str.empty())
        return;

    vector<string> vec;
    strSplitEx(template_str, ",", "\"", "\"", vec);
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        string path = getExtensionAbsolutePath(*it);
        if (isPathDir(path))
        {
            vector<string> files;
            FileFilter ff("");
            listFiles(path, files, &ff);
            for (auto itf = files.begin(); itf != files.end(); ++itf)
            {
                string template_content;
                int begin = 0;
                int end = 0;
                if (Extension::getInstance().templateFileInfo(*itf, template_content, begin, end))
                {
                    templates_[fileBaseName(*itf)] = TemplateInfo(template_content, begin, end);
                }
            }
        }
    }
}

void DocView::refreshSupplementApi()
{
    if (papi_loader_ && ptext_edit_) {
        papi_loader_->loadSupplementApiAsync(
            parse_api_script_,
            kParseSupplementApi,
            getCurrentLine(),
            project_src_dir_);
    }
}

void DocView::apisPreparationFinished()
{
    is_apis_preparing_ = false;
}

void DocView::linesChanged()
{
    resetMarginLineNumberWidth();
}

void DocView::initGui()
{
    QHBoxLayout* pmain_layout = new QHBoxLayout;
    pmain_layout->addWidget(ptext_edit_);
    this->setLayout(pmain_layout);
}

void DocView::resetLexer()
{
    setLexerApi();
}

void DocView::initTextEdit()
{
    ptext_edit_->setUtf8(true);
    ptext_edit_->setAutoIndent(true);
    ptext_edit_->setIndentationsUseTabs(false);
    ptext_edit_->setIndentationWidth(4);
    ptext_edit_->setTabWidth(4);
    ptext_edit_->indicatorDefine(QsciScintilla::RoundBoxIndicator, selection_match_indicator_);
    ptext_edit_->setIndicatorForegroundColor(QColor(251, 220, 0, 120));
    ptext_edit_->setIndentationGuides(true);

    if(tr("") != getPathname())
        ptext_edit_->setText(qtReadFile(getPathname()));
    else
        ptext_edit_->setText(tr(""));
    //text_loaded_ = true;
    ptext_edit_->setModified(false);

    resetLexer();

    ptext_edit_->setMarginLineNumbers (0, true);
    resetMarginLineNumberWidth();
}

bool DocView::testFileFilter(const std::string& file_filter)
{
    std::string title = QStringToStdString(getTitle());
    std::vector<std::string> filterVec;
    util::strSplit(file_filter, ",", filterVec);
    for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
    {
        if (util::strEndWith(title, std::string(".") + *it, false))
            return true;
    }

    return false;
}

void DocView::resetMarginLineNumberWidth()
{
    int lens = ptext_edit_->lines();
    QString str = StdStringToQString(util::strFormat(" %d", lens));
    ptext_edit_->setMarginWidth(0, str);
}

void DocView::init()
{
    initGui();
    initTextEdit();
    initConnections();
}

void DocView::textChanged()
{
    if (ptext_edit_->isModified())
        emit textModified(this);
}

void DocView::initConnections()
{
    connect(getTextEdit(), SIGNAL(textChanged()), this, SLOT(textChanged()));
    connect(ptext_edit_, SIGNAL(linesChanged()), this, SLOT(linesChanged()));
    connect(ptext_edit_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

bool DocView::doSave(bool reset_lexer, const QString& pathname)
{
    bool ret = qtWriteFile(pathname, formatContent(ptext_edit_->text(), true));
    if (ret)
    {
        setPathname(pathname);
        ptext_edit_->setModified(false);
        emit updateTitle(this);
    }

    if (reset_lexer)
        resetLexer();
    else
        refreshSupplementApi();

    return ret;
}

bool DocView::saveDoc()
{
    return getPathname().length() != 0 ? doSave(false, getPathname()) : saveAsDoc();
}

bool DocView::saveAsDoc()
{
    QString pathname = QFileDialog::getSaveFileName(this, "Save File : " + getTitle(), getSaveDialogInitDir(), StdStringToQString(LunarGlobal::getInstance().getFileFilter()));
    return pathname.length() != 0 ? doSave(true, pathname) : false;
}

QString DocView::getTitleFromPath(const QString& path) const
{
    std::string stdpath = QStringToStdString(path);
    return StdStringToQString(util::splitPathname(stdpath).second);
}

QsciLexer* DocView::getLexerFromTypeName(const std::string& type_name, FileType* pout_filetype)
{
    std::string name_trimmed = util::strTrim(type_name);
    if ("" == name_trimmed)
    {
        return new LexerNone(ptext_edit_);
    }

    if (util::strAreEqual(name_trimmed, "avs", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Avs;
        return new QsciLexerAVS(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "bash", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Bash;
        return new QsciLexerBash(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "batch", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Batch;
        return new QsciLexerBatch(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "cmake", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CMake;
        return new QsciLexerCMake(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "coffeescript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CoffeeSript;
        return new QsciLexerCoffeeScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "cpp", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CPP;
        return new QsciLexerCPP(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "csharp", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CSharp;
        return new QsciLexerCSharp(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "css", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CSS;
        return new QsciLexerCSS(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "d", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = D;
        return new QsciLexerD(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "diff", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Diff;
        return new QsciLexerDiff(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "fortran", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Fortran;
        return new QsciLexerFortran(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "fortran77", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Fortran77;
        return new QsciLexerFortran77(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "html", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Html;
        return new QsciLexerHTML(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "idl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Idl;
        return new QsciLexerIDL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "java", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Java;
        return new QsciLexerJava(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "javascript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = JavaScript;
        return new QsciLexerJavaScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "json", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = FileType::Json;
        return new QsciLexerJSON(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "lua", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Lua;
        return new QsciLexerLua(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "makefile", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Makefile;
        return new QsciLexerMakefile(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "matlab", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Matlab;
        return new QsciLexerMatlab(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "markdown", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Markdown;
        return new QsciLexerMarkdown(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "octave", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Octave;
        return new QsciLexerOctave(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "pascal", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Pascal;
        return new QsciLexerPascal(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "perl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Perl;
        return new QsciLexerPerl(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "po", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Po;
        return new QsciLexerPO(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "postscript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = PostScript;
        return new QsciLexerPostScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "pov", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Pov;
        return new QsciLexerPOV(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "properties", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Properties;
        return new QsciLexerProperties(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "python", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Python;
        return new QsciLexerPython(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "ruby", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Ruby;
        return new QsciLexerRuby(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "spice", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Spice;
        return new QsciLexerSpice(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "sql", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Sql;
        return new QsciLexerSQL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "tcl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Tcl;
        return new QsciLexerTCL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "tex", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Tex;
        return new QsciLexerTeX(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "verilog", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Verilog;
        return new QsciLexerVerilog(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "vhdl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Vhdl;
        return new QsciLexerVHDL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "xml", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Xml;
        return new QsciLexerXML(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimmed, "yaml", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Yaml;
        return new QsciLexerYAML(ptext_edit_);
    }
    else
    {
        if (NULL != pout_filetype)
            *pout_filetype = Unknown;
        return new LexerNone(ptext_edit_);
    }
}

bool DocView::find(const QString& expr,
                   bool re,
                   bool cs,
                   bool wo,
                   bool wrap,
                   bool forward,
                   bool first_find,
				   bool from_start
                   )
{
    bool found = false;

	int line = -1;
	int index = -1;
	if(from_start)
	{
		line = 0;
		index = 0;
	}

    if(first_find)
    {
        if (ptext_edit_->hasSelectedText() && ptext_edit_->selectedText() != tr(""))
            found = ptext_edit_->findFirstInSelection(expr, re, cs, wo, forward);
        else
            found = ptext_edit_->findFirst(expr, re, cs, wo, wrap, forward, line, index);
    }
    else
    {
        found = ptext_edit_->findNext();
    }

    return found;
}

void DocView::replace(const QString& replace_with_text)
{
    ptext_edit_->replace(replace_with_text);
}

size_t DocView::getStartSpaceCount(const QString& str)
{
    int i = 0;
    for (i=0; i<str.length(); ++i)
    {
        QChar c = str.at(i);
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
            break;
    }

    return i;
}

void DocView::commentSelection(bool comment_line_or_block)
{
    if (comment_line_or_block)
        commentSelectionLine();
    else
        commentSelectionBlock();
}

void DocView::commentSelectionBlock()
{
    if (comment_block_symbol_begin_.trimmed().length() == 0 || comment_block_symbol_end_ == 0)
        return;

    if (ptext_edit_->selectedText().trimmed().length() == 0)
        return;

    int line_from = 0;
    int index_from = 0;
    int line_to = 0;
    int index_to = 0;
    ptext_edit_->getSelection(&line_from, &index_from, &line_to, &index_to);
    ptext_edit_->findFirstInSelection(ptext_edit_->selectedText(), false, true, false);

    if (ptext_edit_->selectedText().startsWith(comment_block_symbol_begin_) && ptext_edit_->selectedText().endsWith(comment_block_symbol_end_))
    {
        //remove comment
        replace(ptext_edit_->selectedText().mid(comment_block_symbol_begin_.length(),
                                                ptext_edit_->selectedText().length() - comment_block_symbol_begin_.length() - comment_block_symbol_end_.length()));
        int addition = 0;
        if (line_from == line_to)
            addition += comment_block_symbol_begin_.length();
        ptext_edit_->setSelection(line_from, index_from, line_to, index_to - comment_block_symbol_end_.length() - addition);
    }
    else
    {
        //add comment
        replace(comment_block_symbol_begin_ + ptext_edit_->selectedText() + comment_block_symbol_end_);
        int addition = 0;
        if (line_from == line_to)
            addition += comment_block_symbol_begin_.length();
        ptext_edit_->setSelection(line_from, index_from, line_to, index_to + comment_block_symbol_end_.length() + addition);
    }
}

void DocView::commentSelectionLine()
{
    if (comment_line_symbol_.trimmed().length() == 0)
        return;

    int final_line_from = 0;
    int final_line_to = 0;
    if (ptext_edit_->selectedText().length() == 0)
    {
        int line;
        int index;
        ptext_edit_->getCursorPosition(&line, &index);

        //empty line
        if (ptext_edit_->text(line).trimmed().length() == 0)
            return;

        int len = ptext_edit_->text(line).length();
        ptext_edit_->setSelection(line, 0, line, len-1);

        final_line_from = line;
        final_line_to = line;
    }
    else
    {
        int line_from = 0;
        int index_from = 0;
        int line_to = 0;
        int index_to = 0;
        ptext_edit_->getSelection(&line_from, &index_from, &line_to, &index_to);
        int len = ptext_edit_->text(line_to).length();
        ptext_edit_->setSelection(line_from, 0, line_to, len -1);

        final_line_from = line_from;
        final_line_to = line_to;
    }

    QStringList list = formatContent(ptext_edit_->selectedText()).split('\n');

    //check is commented
    bool is_commented = true;
    size_t minium_start_space_count = 0;
    for (int i=0; i<list.size(); ++i)
    {
        QString trimmed_line = list[i].trimmed();
        if (trimmed_line.length() != 0 && !trimmed_line.startsWith(comment_line_symbol_))
            is_commented = false;

        if (i == 0)
        {
            minium_start_space_count = getStartSpaceCount(list[i]);
        }
        else
        {
            size_t count = getStartSpaceCount(list[i]);
            if (minium_start_space_count > count)
                minium_start_space_count = count;
        }
    }

    QString space_prefix = "";
    for (size_t i=0; i<minium_start_space_count; ++i)
        space_prefix.push_back(' ');

    //do comment or uncomment
    for (int i=0; i<list.size(); ++i)
    {
        if (list[i].trimmed().length() != 0)
        {
            if (is_commented)
                list[i] = qstrReplaceOnce(list[i], comment_line_symbol_, tr(""));
            else
                list[i] = qstrReplaceOnce(list[i], space_prefix, space_prefix + comment_line_symbol_);
        }
    }

    QString rep_text = list.join("\n");
    ptext_edit_->findFirstInSelection(ptext_edit_->selectedText(), false, true, false);
    replace(rep_text);

    ptext_edit_->setSelection(final_line_from, 0, final_line_to, ptext_edit_->text(final_line_to).length()-1);
}

QString DocView::getSelectedText() const
{
    return ptext_edit_ ? ptext_edit_->selectedText() : tr("");
}

void DocView::gotoLine(int line)
{
    if (ptext_edit_ && line-1 >=0 && line-1 < ptext_edit_->lines())
        ptext_edit_->setCursorPosition(line-1, 0);
}

void DocView::focusOnEdit()
{
    if (ptext_edit_)
        ptext_edit_->setFocus();
}

bool DocView::getDefinitions(vector<string>& out_results)
{
    if (getPathname().length() == 0 || goto_script_.empty())
        return false;

    QString text;
    if (getSelectedText().trimmed().length() > 0)
    {
        text = getSelectedText().trimmed();
    }
    else
    {
        int line;
        int index;
        ptext_edit_->getCursorPosition(&line, &index);
        if (line < 0 || index < 0)
            return false;

        text = ptext_edit_->wordAtLineIndex(line, index);
        if (text.trimmed().length() == 0)
            return false;
    }

    return GotoManager::getInstance().getDefinitions(
        goto_script_,
        kGotoDefinition,
        QStringToStdString(text),
        getCurrentLine(),
        QStringToStdString(getPathname()),
        project_src_dir_,
        out_results);
}

void DocView::intelligentSelection()
{
    if (ptext_edit_->isCallTipActive())
    {
        ptext_edit_->SCN_CALLTIPCLICK(1);
    }
    else
    {
        int line;
        int index;
        ptext_edit_->getCursorPosition(&line, &index);
        if (line < 0 || index < 0)
            return;

        if (getSelectedText().isEmpty())
        {
            int position = ptext_edit_->positionFromLineIndex(line, index);
            long start_pos = ptext_edit_->SendScintilla(QsciScintilla::SCI_WORDSTARTPOSITION, position, true);
            long end_pos = ptext_edit_->SendScintilla(QsciScintilla::SCI_WORDENDPOSITION, position, true);
            long word_len = end_pos - start_pos;
            if (word_len > 0)
            {
                ptext_edit_->SendScintilla(QsciScintilla::SCI_SETSEL, start_pos, end_pos);
                return;
            }
        }

        QString lineStr = ptext_edit_->text(line);
        QString trimmedLineStr = lineStr.trimmed();
        int start = lineStr.indexOf(trimmedLineStr);
        ptext_edit_->setSelection(line, start, line, start + trimmedLineStr.length());
    }
}

void DocView::gotoLineBeginOrEnd()
{
    if (ptext_edit_->isCallTipActive())
    {
        ptext_edit_->SCN_CALLTIPCLICK(2);
    }
    else
    {
        int line;
        int index;
        ptext_edit_->getCursorPosition(&line, &index);

        QString lineStr = ptext_edit_->text(line);
        QString trimmedLineStr = lineStr.trimmed();
        int start = lineStr.indexOf(trimmedLineStr);
        ptext_edit_->setCursorPosition(line, index == start? start + trimmedLineStr.length() : start);
    }
}

void DocView::codeTemplate()
{
    int line;
    int index;
    ptext_edit_->getCursorPosition(&line, &index);
    if (line < 0 || index < 0)
        return;

    int position = ptext_edit_->positionFromLineIndex(line, index);
    long start_pos = ptext_edit_->SendScintilla(QsciScintilla::SCI_WORDSTARTPOSITION, position, true);
    long end_pos = ptext_edit_->SendScintilla(QsciScintilla::SCI_WORDENDPOSITION, position, true);
    long word_len = end_pos - start_pos;
    if (word_len > 0)
    {
        ptext_edit_->SendScintilla(QsciScintilla::SCI_SETSEL, start_pos, end_pos);
        auto it = templates_.find(QStringToStdString(ptext_edit_->selectedText()));
        if (it != templates_.end())
        {
            ptext_edit_->replaceSelectedText(StdStringToQString(it->second.templateContent()));
            ptext_edit_->setSelection(0, start_pos + it->second.begin(), 0, start_pos + it->second.end());
        }
    }
}

void DocView::zoomIn()
{
    ptext_edit_->zoomIn();
    resetMarginLineNumberWidth();
}

void DocView::zoomOut()
{
    ptext_edit_->zoomOut();
    resetMarginLineNumberWidth();
}

void DocView::zoomToOrigin()
{
    ptext_edit_->zoomTo(0);
    resetMarginLineNumberWidth();
}

void DocView::setEditTextFont(const QFont& font)
{
    LunarGlobal::getInstance().setFont(font);

    if(plexer_)
        plexer_->setFont(font);
    else
        ptext_edit_->setFont(font);

    resetMarginLineNumberWidth();
}

void DocView::selectionChanged()
{
    ptext_edit_->clearIndicatorRange(0, 0, ptext_edit_->lines()-1, ptext_edit_->lineLength(ptext_edit_->lines()-1), selection_match_indicator_);

    if (ptext_edit_->hasSelectedText())
    {
        QString selection = ptext_edit_->selectedText();
        if (selection.trimmed().length() > 0  && !selection.contains("\n"))
        {
            int line_from = 0;
            int index_from = 0;
            int line_to = 0;
            int index_to = 0;
            ptext_edit_->getSelection(&line_from, &index_from, &line_to, &index_to);

            QStringList list = formatContent(ptext_edit_->text()).split('\n');
            for (int i=0; i<list.size(); ++i)
            {
                int pos = 0;
                while ((pos = list.at(i).indexOf(selection, pos)) >= 0)
                {
                    if (line_from != (int)i || line_to != (int)i || index_from != (int)pos || index_to != (int)(pos + selection.length()))
                        ptext_edit_->fillIndicatorRange(i, pos, i ,pos + selection.length(), selection_match_indicator_);
                    pos += selection.length();
                }
            }
        }
    }
}

QString DocView::formatContent(const QString& text, bool trim_empty_line) const
{
    QString txt = text;
    txt.replace("\r\n", "\n").replace("\r", "\n");
    if (!trim_empty_line)
        return txt;

    auto list = txt.split('\n');
    for (int i = 0; i < list.size(); ++i)
        list.replace(i, QStringTrimRight(list.at(i)));

    return list.join('\n');
}

} // namespace gui
