#ifndef GUI_MAINTABWIDGET_H
#define GUI_MAINTABWIDGET_H

#include <QTabWidget>
#include "util/base.hpp"
#include "docview.h"

namespace gui
{

class DocViewWidget;

class MainTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit MainTabWidget(QWidget* parent = 0);
    virtual ~MainTabWidget();
    int addDocViewTab(const QString& pathname);
    std::pair<bool, QString> saveCurDocViewTab(const QString& save_dialog_init_dir = tr("."));
    std::pair<bool, QString> saveAsCurDocViewTab(const QString& save_dialog_init_dir = tr("."));
    void saveAllViewTabs(const QString& save_dialog_init_dir = tr("."));
    void closeCurDocViewTab();
    void closeAllDocViewTabs();
    void setDocViewFont();
    int findTabIndexByFile(const QString& file);
    void forceCloseTab(int index);
    void renameTab(int index, const QString& new_pathname);
    bool findInCurTextEdit(const QString& expr,
                           bool re,
                           bool cs,
                           bool wo,
                           bool wrap,
                           bool forward,
                           bool first_find,
						   bool from_start = false
                           );
    void replaceInCurTextEdit(const QString& replace_with_text);
    bool hasUnsavedFiles();
    void gotoNextTabIndex();
    void gotoPrevTabIndex();
    void currentDocComment(bool comment_line_or_block);
    QString getCurrentDocSelectedText() const;
    QString getCurrentDocPathname() const;
    void currentDocGotoLine(int line);
    void focusOnCurrentDoc();
private Q_SLOTS:
    void tabClose(int index);
    void updateTabTitleAndTip(DocView* pdocview);
    int getTabIndex(const QString& pathname);
private:
    void init();
private:
    DISALLOW_COPY_AND_ASSIGN(MainTabWidget)
};

} // namespace gui

#endif // GUI_MAINTABWIDGET_H
