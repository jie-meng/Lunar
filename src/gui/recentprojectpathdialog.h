#ifndef RECENTPROJECTPATHDIALOG_H
#define RECENTPROJECTPATHDIALOG_H

#include <QDialog>
#include "util/base.hpp"

class QLabel;
class QPushButton;

namespace gui
{

class TreeView;

class RecentProjectPathDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecentProjectPathDialog(QWidget *parent = 0);
    virtual ~RecentProjectPathDialog();
    void init();
    void initConnections();
    void initGui();
signals:
    void selectRecentProjectPath(const QString&);
    void newProjectPath();
public slots:
    void onSelectRecentProjectPathItem(const QStringList& item, int number);
    void onNewProjectPath();
private:
    QLabel *plabel_recent_;
    TreeView* ptree_view_;
    QPushButton *pnew_button_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentProjectPathDialog)
};

} // namespace gui

#endif // RECENTPROJECTPATHDIALOG_H
