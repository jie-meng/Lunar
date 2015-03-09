#include "docview.h"
#include <fstream>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexeroctave.h>
#include "util/string.hpp"
#include "util/file.hpp"
#include "util/regex.hpp"
#include "util/cfg.hpp"
#include "lunarapi.h"
#include "octaveapi.h"
#include "lunarcommon.h"
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
    file_type_(Unknown)
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
}

void DocView::SetUnknownLexerApi()
{
    ClearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    ptext_edit_->setFont(LunarGlobal::get_font());
    if(LunarGlobal::get_autocompletion_wordtip())
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    else
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    ptext_edit_->setLexer(NULL);
    ptext_edit_->setAutoCompletionCaseSensitivity(false);

    file_type_ = Unknown;
}

void DocView::SetLuaLexerApi()
{
    ClearLexerApi();

    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    plexer_ = new QsciLexerLua(ptext_edit_);
    plexer_->setFont(LunarGlobal::get_font());

    papis_ = new LunarApi(plexer_);
    if(LunarGlobal::get_autocompletion_wordtip())
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    }
    else
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    }

    plexer_->setAPIs(papis_);
    ptext_edit_->setLexer(plexer_);
    ptext_edit_->setAutoCompletionCaseSensitivity(false);

    //load APIs
    papi_loader_ = new ApiLoaderLua(QStringToStdString(pathname_), papis_, this);
    papi_loader_->LoadFileApis(LunarGlobal::get_app_path() + LunarGlobal::getLuaApi());
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
    plexer_ = new QsciLexerOctave(ptext_edit_);
    plexer_->setFont(LunarGlobal::get_font());

    papis_ = new OctaveApi(plexer_);
    if(LunarGlobal::get_autocompletion_wordtip())
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
    }
    else
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    }

    plexer_->setAPIs(papis_);
    ptext_edit_->setLexer(plexer_);
    ptext_edit_->setAutoCompletionCaseSensitivity(false);

    //load APIs
    papi_loader_ = new ApiLoaderOctave(QStringToStdString(pathname_), papis_, this);
    papi_loader_->LoadFileApis(LunarGlobal::get_app_path() + LunarGlobal::getOctaveApi());
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
