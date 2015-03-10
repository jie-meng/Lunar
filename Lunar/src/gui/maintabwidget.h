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
    int AddDocViewTab(const QString& pathname);
    bool SaveCurDocViewTab();
    bool SaveAsCurDocViewTab();
    void SaveAllViewTabs();
    void CloseCurDocViewTab();
    void SetDocViewFont();
    bool FindInCurTextEdit(const QString& expr,
                           bool re,
                           bool cs,
                           bool wo,
                           bool wrap,
                           bool forward,
                           bool first_find,
						   bool from_start = false
                           );
    void ReplaceInCurTextEdit(const QString& replace_with_text);
    bool HasUnsavedFiles();
private Q_SLOTS:
    void UpdateTabTitleAndTip(DocView*);
    void TabTextModified(DocView*);
    void TabClose(int index);
    int GetTabIndex(const QString& pathname);
private:
    void Init();
private:
    DISALLOW_COPY_AND_ASSIGN(MainTabWidget)
};

} // namespace gui

#endif // GUI_MAINTABWIDGET_H
