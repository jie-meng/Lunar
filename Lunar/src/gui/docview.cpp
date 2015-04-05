#include "docview.h"
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
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerd.h>
#include <Qsci/qscilexerdiff.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexermatlab.h>
#include <Qsci/qscilexeroctave.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerproperties.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
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
#include "octaveapi.h"
#include "lunarcommon.h"
#include "extension.h"
#include "apiloaderoctave.h"
#include "apiloaderlua.h"
#include "mainwindow.h"

namespace gui
{

int DocView::s_new_docview_sequence_ = 0;

DocView::DocView(const QString& pathname, QWidget* parent)
    : QWidget(parent),
    pathname_(pathname),
    papis_(NULL),
    plexer_(NULL),
    is_apis_preparing_(false),
    papi_loader_(NULL),
    file_type_(Unknown),
    executor_("")
{
    //ctor
    ptext_edit_ = new QsciScintilla(parent);
    title_ = GetTitleFromPath(pathname_);
    if(tr("") == title_)
    {
        std::string name = util::strFormat("New_%d", DocView::s_new_docview_sequence_++);
        title_ = StdStringToQString(name);
    }
    Init();
}

DocView::~DocView()
{
    //dtor
    ClearLexerApi();
}

void DocView::ClearLexerApi()
{
    if (plexer_)
        util::safeDelete(plexer_);
    //papis_ would be delete together with plexer_

    if (papi_loader_)
        util::safeDelete(papi_loader_);

    executor_ = "";
    file_type_ = Unknown;
}

void DocView::SetUnknownLexerApi()
{
    ClearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    ptext_edit_->setFont(LunarGlobal::get_font());
    ptext_edit_->setAutoCompletionCaseSensitivity(false);
    if(LunarGlobal::get_autocompletion_wordtip())
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    else
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    //Extension
    if (Extension::getInstance().isOk())
    {
        std::string filename = QStringToStdString(pathname_);
        std::string type = "";
        std::string api = "";
        std::string executor = "";
        if (Extension::getInstance().parse(filename, &type, &api, &executor))
        {
            FileType filetype = Unknown;
            plexer_ = GetLexerFromTypeName(type, &filetype);
            if (NULL != plexer_)
            {
                ptext_edit_->setLexer(plexer_);
                plexer_->setFont(LunarGlobal::get_font());
                if (NULL == plexer_->apis())
                {
                    papis_ = new QsciAPIsEx(plexer_);
                    plexer_->setAPIs(papis_);
                }
                else
                {
                    papis_ = plexer_->apis();
                }
                ptext_edit_->setLexer(plexer_);

                if (util::isPathDir(LunarGlobal::get_app_path() + "/" + api))
                {
                    papi_loader_ = new ApiLoader(QStringToStdString(pathname_), (QsciAPIsEx*)papis_, this);
                    papi_loader_->LoadFileApis(LunarGlobal::get_app_path() + "/" + api);
                    papi_loader_->Prepare();
                }

                //set executor
                executor_ = executor;

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

void DocView::SetLuaLexerApi()
{
    ClearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    ptext_edit_->setAutoCompletionCaseSensitivity(false);
    if(LunarGlobal::get_autocompletion_wordtip())
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    else
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    plexer_ = new QsciLexerLua(ptext_edit_);
    plexer_->setFont(LunarGlobal::get_font());
    papis_ = new LunarApi(plexer_);
    plexer_->setAPIs(papis_);
    ptext_edit_->setLexer(plexer_);

    //load APIs
    papi_loader_ = new ApiLoaderLua(QStringToStdString(pathname_), (QsciAPIsEx*)papis_, this);
    papi_loader_->LoadFileApis(LunarGlobal::get_app_path() + "/" + LunarGlobal::getLuaApi());
    papi_loader_->ParseCurrentFileApi();
    papi_loader_->AppendApiCurrentFile();
    papi_loader_->ParseIncludeFileApi();
    papi_loader_->AppendApiIncludeFile();
    papi_loader_->Prepare();

    file_type_ = Lua;
}

void DocView::SetOctaveLexerApi()
{
    ClearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    ptext_edit_->setAutoCompletionCaseSensitivity(false);
    if(LunarGlobal::get_autocompletion_wordtip())
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    else
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    plexer_ = new QsciLexerOctave(ptext_edit_);
    plexer_->setFont(LunarGlobal::get_font());
    papis_ = new OctaveApi(plexer_);
    plexer_->setAPIs(papis_);
    ptext_edit_->setLexer(plexer_);

    //load APIs
    papi_loader_ = new ApiLoaderOctave(QStringToStdString(pathname_), (QsciAPIsEx*)papis_, this);
    papi_loader_->LoadFileApis(LunarGlobal::get_app_path() + "/" + LunarGlobal::getOctaveApi());
    papi_loader_->ParseCurrentFileApi();
    papi_loader_->AppendApiCurrentFile();
    papi_loader_->ParseIncludeFileApi();
    papi_loader_->AppendApiIncludeFile();
    papi_loader_->Prepare();

    file_type_ = Octave;
}

void DocView::ApisPreparationFinished()
{
    is_apis_preparing_ = false;
}

void DocView::LinesChanged()
{
    ResetMarginLineNumberWidth();
}

void DocView::InitGui()
{
    QHBoxLayout* pmain_layout = new QHBoxLayout;
    pmain_layout->addWidget(ptext_edit_);
    this->setLayout(pmain_layout);
}

void DocView::ResetLexer()
{
    if (TestFileFilter(LunarGlobal::getLuaFileFilter()))
        SetLuaLexerApi();
    else if (TestFileFilter((LunarGlobal::getOctaveFileFilter())))
        SetOctaveLexerApi();
    else
        SetUnknownLexerApi();
}

void DocView::InitTextEdit()
{
    ptext_edit_->setUtf8(true);
    ptext_edit_->setAutoIndent(true);
    ptext_edit_->setIndentationsUseTabs(false);
    ptext_edit_->setIndentationWidth(4);
    ptext_edit_->setTabWidth(4);

    if(tr("") != pathname_)
    {
        ptext_edit_->setText(StdStringToQString(util::readTextFile(QStringToStdString(pathname_))));
        ResetLexer();
    }
    else
    {
        ptext_edit_->setText(tr(""));
        SetUnknownLexerApi();
    }
    ptext_edit_->setMarginLineNumbers (0, true);
    ResetMarginLineNumberWidth();
}

bool DocView::TestFileFilter(const std::string& file_filter)
{
    std::string title = QStringToStdString(title_);
    std::vector<std::string> filterVec;
    util::strSplit(file_filter, ",", filterVec);
    for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
    {
        if (util::strEndWith(title, std::string(".") + *it, false))
            return true;
    }

    return false;
}

void DocView::ResetMarginLineNumberWidth()
{
    int lens = ptext_edit_->lines();
    QString str = StdStringToQString(util::strFormat(" %d", lens));
    ptext_edit_->setMarginWidth(0, str);
}

void DocView::Init()
{
    InitGui();
    InitTextEdit();
    InitConnections();
}

void DocView::TextChanged()
{
    if (get_text_edit()->isModified())
    {
        emit TextModified(this);
    }
}

void DocView::InitConnections()
{
    connect(get_text_edit(), SIGNAL(textChanged()), this, SLOT(TextChanged()));
    connect(ptext_edit_, SIGNAL(linesChanged()), this, SLOT(LinesChanged()));
    connect(papis_, SIGNAL(apiPreparationFinished()), this, SLOT(ApisPreparationFinished()));
}

bool DocView::DoSave()
{
    emit UpdateTitle(this);
    std::string content = QStringToStdString(ptext_edit_->text());
    content = util::strReplaceAll(content, "\r\n", "\n");
    ResetLexer();

    return util::writeTextFile(QStringToStdString(pathname_), content);
}

bool DocView::SaveDoc()
{
    if (tr("") != pathname_)
        return DoSave();
    else
        return SaveAsDoc();
}

bool DocView::SaveAsDoc()
{
    //newed file
    QString title = "Save File : " + get_title();
    QString path = QFileDialog::getSaveFileName(this, title, ".", StdStringToQString(MainWindow::get_file_filter()));
    if(path.length() == 0)
    {
        //do nothing
        return false;
    }
    else
    {
        pathname_ = path;
        title_ = GetTitleFromPath(pathname_);
        return DoSave();
    }
}

QString DocView::GetTitleFromPath(const QString& path) const
{
    std::string stdpath = QStringToStdString(path);
    return StdStringToQString(util::splitPathname(stdpath).second);
}

QsciLexer* DocView::GetLexerFromTypeName(const std::string& type_name, FileType* pout_filetype)
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
    else if (util::strAreEqual(name_trimed, "html", false))
    {
        if (NULL != pout_filetype)
            *pout_filetype = Html;
        return new QsciLexerHTML(ptext_edit_);
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
        QsciLexer* pl = new QsciLexerLua(ptext_edit_);

        if (NULL != pout_filetype)
            *pout_filetype = Lua;

        pl->setAPIs(new LunarApi(pl));

        return pl;
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
        QsciLexer* pl = new QsciLexerOctave(ptext_edit_);

        if (NULL != pout_filetype)
            *pout_filetype = Octave;

        pl->setAPIs(new OctaveApi(pl));

        return pl;
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

bool DocView::Find(const QString& expr,
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
        found = ptext_edit_->findFirst(expr, re, cs, wo, wrap, forward, line, index);
    }
    else
    {
        found = ptext_edit_->findNext();
    }

    return found;
}

void DocView::Replace(const QString& replace_with_text)
{
    ptext_edit_->replace(replace_with_text);
}

void DocView::SetEditTextFont(const QFont& font)
{
    LunarGlobal::set_font(font);

    if(plexer_)
        plexer_->setFont(font);
    else
        ptext_edit_->setFont(font);

    ResetMarginLineNumberWidth();
}

} // namespace gui
