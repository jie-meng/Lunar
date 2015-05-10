#ifndef SEARCHRESULTSWIDGET_H
#define SEARCHRESULTSWIDGET_H

#include <QTreeWidget>
#include <string>
#include "util/file.hpp"
#include "util/regex.hpp"

class QKeyEvent;

namespace gui
{

class SearchResultsWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SearchResultsWidget(QWidget *parent = 0);
    ~SearchResultsWidget();

signals:
    void gotoSearchResult(const QString&, int);
public slots:
    void searchInPath(const QString& path,
                      const QString& text,
                      const QString& exts,
                      bool case_sensitive = true,
                      bool use_regexp = false);
protected:
    virtual void keyPressEvent(QKeyEvent *event);
private slots:
    void onItemReturn(QTreeWidgetItem *item, int column);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
private:
    void initConnections();
    void searchInFile(const std::string& file, const std::string& text, bool case_sensitive, bool use_regexp);
    void searchInDirectory(const std::string& dir, util::PathFilter* path_filter, const std::string& text, bool case_sensitive, bool use_regexp);
private:
    util::Regex regex_;
};

}

#endif // SEARCHRESULTSWIDGET_H
