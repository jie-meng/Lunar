#ifndef SEARCHRESULTSWIDGET_H
#define SEARCHRESULTSWIDGET_H

#include <QTreeWidget>
#include <QThread>
#include <string>
#include "util/file.hpp"
#include "util/regex.hpp"

class QKeyEvent;

namespace gui
{

class SearchThread : public QThread
{
    Q_OBJECT
public:
    SearchThread(QObject *parent = 0);
    virtual ~SearchThread();
    void start(const QString& path,
               const QString& text,
               const QString& exts,
               bool case_sensitive,
               bool use_regexp);
signals:
    void found(const QString&, const QString&, const QString&);
protected:
    virtual void run();
private:
    void searchInFile(const std::string& file, const std::string& text, bool case_sensitive, bool use_regexp);
    void searchInDirectory(const std::string& dir, util::PathFilter* path_filter, const std::string& text, bool case_sensitive, bool use_regexp);
private:
    QString path_;
    QString text_;
    QString exts_;
    bool case_sensitive_;
    bool use_regexp_;
    util::Regex regex_;
};

class SearchResultsWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SearchResultsWidget(QWidget *parent = 0);
    virtual ~SearchResultsWidget();

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
    void onAddItem(const QString& file, const QString& line, const QString& text);
    void onItemReturn(QTreeWidgetItem *item, int column);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
private:
    void initConnections();
    void searchInFile(const std::string& file, const std::string& text, bool case_sensitive, bool use_regexp);
    void searchInDirectory(const std::string& dir, util::PathFilter* path_filter, const std::string& text, bool case_sensitive, bool use_regexp);
private:
    SearchThread search_thread_;
};

}

#endif // SEARCHRESULTSWIDGET_H
