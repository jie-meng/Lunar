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
    bool saveCurDocViewTab();
    bool saveAsCurDocViewTab();
    void saveAllViewTabs();
    void closeCurDocViewTab();
    void setDocViewFont();
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
private Q_SLOTS:
    void updateTabTitleAndTip(DocView*);
    void tabTextModified(DocView*);
    void tabClose(int index);
    int getTabIndex(const QString& pathname);
private:
    void init();
private:
    DISALLOW_COPY_AND_ASSIGN(MainTabWidget)
};

} // namespace gui

#endif // GUI_MAINTABWIDGET_H
