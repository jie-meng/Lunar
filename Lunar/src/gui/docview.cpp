#include "docview.h"
#include <fstream>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qsciapis.h>
#include "util/string.hpp"
#include "util/file.hpp"
#include "util/regex.hpp"
#include "util/cfg.hpp"
#include "lunarcommon.h"
#include "apiloader.h"
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
    papi_loader_(NULL)
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
}

void DocView::SetLuaLexerAndPslApi()
{
    ptext_edit_->setAutoCompletionThreshold(LunarGlobal::get_autocompletion_threshold());
    plexer_ = new QsciLexerLua(ptext_edit_);
    plexer_->setFont(LunarGlobal::get_font());

    papis_ = new QsciAPIs(plexer_);
    if(LunarGlobal::get_autocompletion_wordtip())
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAll);
        papis_->setAutoCompletionApiTip(true);
    }
    else
    {
        ptext_edit_->setAutoCompletionSource(QsciScintilla::AcsAPIs);
        papis_->setAutoCompletionApiTip(false);
    }

    plexer_->setAPIs(papis_);
    ptext_edit_->setLexer(plexer_);
    ptext_edit_->setAutoCompletionCaseSensitivity(false);

    //load APIs
    papi_loader_ = new ApiLoader(ptext_edit_, papis_, this);
    papi_loader_->LoadFileApis();
    //not stable
    //papi_loader_->LoadFileObjApis();
    papi_loader_->Prepare();
}

void DocView::ApisPreparationFinished()
{
    is_apis_preparing_ = false;
}

void DocView::LinesChanged()
{
    ResetMarginLineNumberWidth();
    UpdateApis();
}

void DocView::ParseRequireFiles(const std::string& filepath)
{
    util::printLine(filepath);
}

void DocView::UpdateApisProc()
{

//    //clear tmp apis
    if (papi_loader_)
    {
        papi_loader_->ClearTmpApis();
        papi_loader_->ParseCurrentTextApis();
        //not stable
        //papi_loader_->ParseCurrentTextObjApis();
        papi_loader_->AppendTmpApis();

        //start prepare
        papi_loader_->Prepare();
    }
}

void DocView::UpdateApis()
{
    if(!is_apis_preparing_)
    {
        is_apis_preparing_ = true;
        UpdateApisProc();
    }
}

void DocView::InitGui()
{
    QHBoxLayout* pmain_layout = new QHBoxLayout;
    pmain_layout->addWidget(ptext_edit_);
    this->setLayout(pmain_layout);
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

        std::string title = QStringToStdString(title_);
        std::vector<std::string> filterVec;
        util::strSplit(LunarGlobal::getFileFilter(), ",", filterVec);
        for (std::vector<std::string>::iterator it = filterVec.begin(); it != filterVec.end(); ++it)
        {

            if (util::strEndWith(title, *it, false))
            {
                SetLuaLexerAndPslApi();
                break;
            }
        }

//        if (title_.endsWith(".lua", Qt::CaseInsensitive)
//            || title_.endsWith(".psl", Qt::CaseInsensitive)
//            || title_.endsWith(".script", Qt::CaseInsensitive))
//        {
//            SetLuaLexerAndPslApi();
//        }
    }
    else
    {
        ptext_edit_->setText(tr(""));
        SetLuaLexerAndPslApi();
    }
    ptext_edit_->setMarginLineNumbers (0, true);
    ResetMarginLineNumberWidth();
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
//    ptext_edit_->setFocus();
//    ptext_edit_->setCursorPosition(0, 0);
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
