#include "searchresultswidget.h"
#include <fstream>
#include <algorithm>
#include <QKeyEvent>
#include "util/lexicalcast.hpp"
#include "lunarcommon.h"
#include "extension.h"

using namespace std;
using namespace util;

namespace gui
{

//SearchFileFilter
class SearchFileFilter : public PathFilter
{
public:
    explicit SearchFileFilter(const std::string& exts = "")
    {
        string format_exts = strTrim(exts);
        if (!format_exts.empty())
            strSplit(format_exts, ",", vec_);
    }

    virtual bool filter(const std::string& path)
    {
        if (!isPathFile(path))
            return false;

        if (vec_.empty())
            return true;

        for (vector<string>::iterator it = vec_.begin(); it != vec_.end(); ++it)
        {
            if (strAreEqual(fileExtension(path), strTrim(*it), false) && Extension::getInstance().isLegalFile(path)) 
                return true;
        }
        return false;
    }
private:
    vector<string> vec_;
};

//SearchThread
SearchThread::SearchThread(QObject *parent) : QThread(parent)
{
}

void SearchThread::start(const QString& path,
           const QString& text,
           const QString& exts,
           bool case_sensitive,
           bool use_regexp)
{
    path_ = path;
    text_ = text;
    exts_ = exts;
    case_sensitive_ = case_sensitive;
    use_regexp_ = use_regexp;

    QThread::start();
}

SearchThread::~SearchThread()
{
}

void SearchThread::run()
{
    //for efficiency, set regex as an object of SearchResultsWidget instead of create on stack whenever use
    if (use_regexp_)
    {
        Regex::RegexFlag flag = case_sensitive_ ? Regex::NoFlag : Regex::IgnoreCase;
        regex_.compile(QStringToStdString(text_), flag);
    }

    string str_path = QStringToStdString(path_);
    string str_text = QStringToStdString(text_);

    if (isPathFile(str_path))
    {
        searchInFile(str_path, str_text, case_sensitive_, use_regexp_);
    }
    else if (isPathDir(str_path))
    {
        SearchFileFilter sff(QStringToStdString(exts_));
        searchInDirectory(str_path, &sff, str_text, case_sensitive_, use_regexp_);
    }
}

void SearchThread::searchInFile(const std::string& file, const std::string& text, bool case_sensitive, bool use_regexp)
{
    string content = QStringToStdString(qtReadFile(StdStringToQString(file)));
    if (content.empty())
        return;

    content = strReplaceAll(content, "\r\n", "\n");
    content = strReplaceAll(content, "\r", "\n");

    vector<string> vec;
    strSplit(content, "\n", vec);
    for (size_t i = 0; i<vec.size(); ++i)
    {
        //LogSocket::getInstance().sendLog(strFormat("%d -- %s", i+1, vec[i].c_str()), "127.0.0.1", 9966);

        bool match = false;
        if (!use_regexp)
        {
            if (strContains(vec[i], text, case_sensitive))
                match = true;
        }
        else
        {
            if (regex_.search(vec[i]))
                match = true;
        }

        if (match)
        {
            Q_EMIT found(StdStringToQString(strReplace(file, currentPath() + "/", "")), StdStringToQString(toString(i+1)), StdStringToQString(strTrim(vec[i])));
        }
    }
}

void SearchThread::searchInDirectory(const std::string& dir, PathFilter* path_filter, const std::string& text, bool case_sensitive, bool use_regexp)
{
    vector<string> vec;
    listFiles(dir, vec, path_filter);
    if (!vec.empty())
    {
        sort(vec.begin(), vec.end());
        for (vector<string>::iterator it = vec.begin(); it != vec.end(); ++it)
            searchInFile(*it, text, case_sensitive, use_regexp);
    }

    DirFilter df;
    vec.clear();
    listFiles(dir, vec, &df);
    if (!vec.empty())
    {
        sort(vec.begin(), vec.end());
        for (vector<string>::iterator it = vec.begin(); it != vec.end(); ++it)
            searchInDirectory(*it, path_filter, text, case_sensitive, use_regexp);
    }
}

SearchResultsWidget::SearchResultsWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    setColumnCount(2);
    setColumnWidth(0, 200);
    setColumnWidth(1, 50);

    QStringList list;
    list.append("File");
    list.append("Line");
    list.append("Text");
    setHeaderLabels(list);

    initConnections();
}

SearchResultsWidget::~SearchResultsWidget()
{
}

void SearchResultsWidget::searchInPath(const QString& path,
                                       const QString& text,
                                       const QString& exts,
                                       bool case_sensitive,
                                       bool use_regexp)
{
    if (search_thread_.isRunning())
    {
        LunarMsgBoxQ("Search is running.");
        return;
    }

    clear();
    
    if (text.isEmpty())
        return;

    search_thread_.start(path, text, exts, case_sensitive, use_regexp);
}

void SearchResultsWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        onItemReturn(currentItem(), currentColumn());
        break;
    default:
        break;
    }

    QTreeWidget::keyPressEvent(event);
}

void SearchResultsWidget::initConnections()
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));
    connect(&search_thread_, SIGNAL(found(const QString&, const QString&, const QString&)), this, SLOT(addItem(const QString&, const QString&, const QString&)));
    connect(&search_thread_, SIGNAL(finished()), this, SLOT(findFinish()));
}

void SearchResultsWidget::addItem(const QString& file, const QString& line, const QString& text)
{
    QStringList list;
    list.append(file);
    list.append(line);
    list.append(text);
    QTreeWidgetItem* pnode = new QTreeWidgetItem((QTreeWidget*)0, list);
    addTopLevelItem(pnode);
}


void SearchResultsWidget::findFinish()
{   
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
}

void SearchResultsWidget::onItemReturn(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

   Q_EMIT gotoSearchResult(item->text(0), lexicalCastDefault<size_t>(QStringToStdString(item->text(1)), 1));
}

void SearchResultsWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

    Q_EMIT gotoSearchResult(item->text(0), lexicalCastDefault<size_t>(QStringToStdString(item->text(1)), 1));
}

}
