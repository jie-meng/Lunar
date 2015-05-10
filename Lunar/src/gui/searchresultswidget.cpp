#include "searchresultswidget.h"
#include <fstream>
#include <algorithm>
#include <QKeyEvent>
#include "lexicalcast.hpp"
#include "lunarcommon.h"
#include "extension.h"

using namespace std;
using namespace util;

namespace gui
{

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
        if (!isPathFile(path) || Extension::getInstance().ignoreFile(path))
            return false;

        if (vec_.empty())
            return true;

        for (vector<string>::iterator it = vec_.begin(); it != vec_.end(); ++it)
        {
            if (strAreEqual(fileExtension(path), strTrim(*it), false))
                return true;
        }
        return false;
    }
private:
    vector<string> vec_;
};

SearchResultsWidget::SearchResultsWidget(QWidget *parent) : QTreeWidget(parent)
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
    //clear all items
    clear();

    //for efficiency, set regex as an object of SearchResultsWidget instead of create on stack whenever use
    if (use_regexp)
    {
        Regex::RegexFlag flag = case_sensitive ? Regex::NoFlag : Regex::IgnoreCase;
        regex_.compile(QStringToStdString(text), flag);
    }

    string str_path = QStringToStdString(path);
    string str_text = QStringToStdString(text);

    if (isPathFile(str_path))
    {
        searchInFile(str_path, str_text, case_sensitive, use_regexp);
    }
    else if (isPathDir(str_path))
    {
        SearchFileFilter sff(QStringToStdString(exts));
        searchInDirectory(str_path, &sff, str_text, case_sensitive, use_regexp);
    }
}

void SearchResultsWidget::searchInFile(const std::string& file, const std::string& text, bool case_sensitive, bool use_regexp)
{
    std::ifstream ifs(file.c_str());
    if (ifs.is_open())
    {
        std::string line("");
        size_t idx = 0;
        while(!ifs.eof())
        {
            std::getline(ifs, line);
            ++idx;
            bool match = false;
            if (!use_regexp)
            {
                if (strContains(line, text, case_sensitive))
                    match = true;
            }
            else
            {
                if (regex_.search(line))
                    match = true;
            }

            if (match)
            {
                QStringList list;
                list.append(StdStringToQString(strReplace(file, currentPath() + "/", "")));
                list.append(StdStringToQString(toString(idx)));
                list.append(StdStringToQString(strTrim(line)));
                QTreeWidgetItem* pnode = new QTreeWidgetItem((QTreeWidget*)0, list);
                addTopLevelItem(pnode);
            }
        }
        ifs.close();
    }
}

void SearchResultsWidget::searchInDirectory(const std::string& dir, PathFilter* path_filter, const std::string& text, bool case_sensitive, bool use_regexp)
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
