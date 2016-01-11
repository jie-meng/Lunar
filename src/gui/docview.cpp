#include "docview.h"
#include <vector>
#include <fstream>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <Qsci/qsciabstractapis.h>
#include <Qsci/qsciscintilla.h>
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
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexermatlab.h>
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
#include "util/file.hpp"
#include "util/regex.hpp"
#include "util/cfg.hpp"
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
    parse_supplement_api_script_(""),
    parse_supplement_api_func_(""),
    project_src_dir_(""),
    goto_script_(""),
    goto_definition_func_(""),
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
    return line+1;
}

void DocView::focusOnText()
{
    ptext_edit_->setFocus();
}

void DocView::clearLexerApi()
{
    if (plexer_)
        safeDelete(plexer_);
    //papis_ would be delete together with plexer_

    if (papi_loader_)
        safeDelete(papi_loader_);

    executor_ = "";
    parse_supplement_api_script_ = "";
    parse_supplement_api_func_ = "";
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
            size_t auto_complete_type_ = getValueFromMap<size_t>(dict, "auto_complete_type", 0);
            executor_ = getValueFromMap<string>(dict, "executor", "");
            parse_supplement_api_script_ = getValueFromMap<string>(dict, "parse_supplement_api_script", "");
            parse_supplement_api_func_ = getValueFromMap<string>(dict, "parse_supplement_api_func", "");
            project_src_dir_ = getValueFromMap<string>(dict, "project_src_dir", "");
            goto_script_ = getValueFromMap<string>(dict, "goto_script", "");
            goto_definition_func_ = getValueFromMap<string>(dict, "goto_definition_func", "");
            comment_line_symbol_ = StdStringToQString(getValueFromMap<string>(dict, "comment_line", ""));
            comment_block_symbol_begin_ = StdStringToQString(getValueFromMap<string>(dict, "comment_block_begin", ""));
            comment_block_symbol_end_ = StdStringToQString(getValueFromMap<string>(dict, "comment_block_end", ""));

            FileType filetype = Unknown;
            plexer_ = getLexerFromTypeName(getValueFromMap<string>(dict, "type", ""), &filetype);
            if (NULL != plexer_)
            {
                ptext_edit_->setLexer(plexer_);
                plexer_->setFont(LunarGlobal::getInstance().getFont());
                if (0 == auto_complete_type_)
                    papis_ = new QsciAPIsEx(plexer_);
                else
                    papis_ = new LunarApi(plexer_);
                plexer_->setAPIs(papis_);
                ptext_edit_->setLexer(plexer_);

                //api
                papi_loader_ = new ApiLoader(papis_, QStringToStdString(pathname_));
                papi_loader_->loadCommonApiAsync(getValueFromMap<string>(dict, "api", ""));
                //do not load supplement api when first load, because it'll not work until loadCommonApiAsync ended.
                //papi_loader_->loadSupplementApiAsync(parse_supplement_api_script_, parse_supplement_api_func_, getCurrentLine());

                //parse success
                file_type_ = filetype;
                return;
            }
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

void DocView::refreshSupplementApi()
{
    if (papi_loader_ && ptext_edit_)
        papi_loader_->loadSupplementApiAsync(
            parse_supplement_api_script_,
            parse_supplement_api_func_,
            getCurrentLine(),
            project_src_dir_);
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
    //connect(ptext_edit_, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorPositionChanged(int, int)))
}

bool DocView::doSave(bool reset_lexer, const QString& pathname)
{
    bool ret = qtWriteFile(pathname, removeTextReturn(ptext_edit_->text()));
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
    std::string name_trimed = util::strTrim(type_name);
    if ("" == name_trimed)
        return NULL;

    if (util::strAreEqual(name_trimed, "avs", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Avs;
        return new QsciLexerAVS(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "bash", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Bash;
        return new QsciLexerBash(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "batch", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Batch;
        return new QsciLexerBatch(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "cmake", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CMake;
        return new QsciLexerCMake(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "coffeescript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CoffeeSript;
        return new QsciLexerCoffeeScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "cpp", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CPP;
        return new QsciLexerCPP(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "csharp", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CSharp;
        return new QsciLexerCSharp(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "css", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = CSS;
        return new QsciLexerCSS(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "d", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = D;
        return new QsciLexerD(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "diff", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Diff;
        return new QsciLexerDiff(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "fortran", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Fortran;
        return new QsciLexerFortran(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "fortran77", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Fortran77;
        return new QsciLexerFortran77(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "html", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Html;
        return new QsciLexerHTML(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "idl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Idl;
        return new QsciLexerIDL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "java", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Java;
        return new QsciLexerJava(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "javascript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = JavaScript;
        return new QsciLexerJavaScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "lua", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Lua;
        return new QsciLexerLua(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "makefile", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Makefile;
        return new QsciLexerMakefile(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "matlab", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Matlab;
        return new QsciLexerMatlab(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "octave", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Octave;
        return new QsciLexerOctave(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "pascal", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Pascal;
        return new QsciLexerPascal(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "perl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Perl;
        return new QsciLexerPerl(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "po", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Po;
        return new QsciLexerPO(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "postscript", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = PostScript;
        return new QsciLexerPostScript(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "pov", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Pov;
        return new QsciLexerPOV(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "properties", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Properties;
        return new QsciLexerProperties(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "python", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Python;
        return new QsciLexerPython(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "ruby", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Ruby;
        return new QsciLexerRuby(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "spice", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Spice;
        return new QsciLexerSpice(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "sql", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Sql;
        return new QsciLexerSQL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "tcl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Tcl;
        return new QsciLexerTCL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "tex", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Tex;
        return new QsciLexerTeX(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "verilog", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Verilog;
        return new QsciLexerVerilog(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "vhdl", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Vhdl;
        return new QsciLexerVHDL(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "xml", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Xml;
        return new QsciLexerXML(ptext_edit_);
    }
    else if (util::strAreEqual(name_trimed, "yaml", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Yaml;
        return new QsciLexerYAML(ptext_edit_);
    }
    else
    {
        if (NULL != pout_filetype)
            *pout_filetype = Unknown;
        return NULL;
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

    QStringList list = removeTextReturn(ptext_edit_->selectedText()).split('\n');

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
    if (ptext_edit_)
        return ptext_edit_->selectedText();
    else
        return tr("");
}

void DocView::gotoLine(int line)
{
    if (ptext_edit_)
    {
        if (line-1 >=0 && line-1<ptext_edit_->lines())
            ptext_edit_->setCursorPosition(line-1, 0);
    }
}

void DocView::focusOnEdit()
{
    if (ptext_edit_)
        ptext_edit_->setFocus();
}

bool DocView::getDefinitions(vector<string>& out_results)
{
    if (getPathname().length() == 0 || goto_script_.empty() || goto_definition_func_.empty())
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
        goto_definition_func_,
        QStringToStdString(text),
        getCurrentLine(),
        QStringToStdString(getPathname()),
        project_src_dir_,
        out_results);
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

            QStringList list = removeTextReturn(ptext_edit_->text()).split('\n');
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

QString DocView::removeTextReturn(const QString& text) const
{
    QString txt = text;
    return txt.replace("\r\n", "\n").replace("\r", "\n");
}

} // namespace gui
