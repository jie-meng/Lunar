#ifndef RECENTPROJECTPATHDIALOG_H
#define RECENTPROJECTPATHDIALOG_H

#include <QDialog>
#include "util/base.hpp"

class QLabel;

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
public slots:
    void onSelectRecentProjectPathItem(const QStringList& item, int number);
private:
    TreeView* ptree_view_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentProjectPathDialog)
};

} // namespace gui

#endif // RECENTPROJECTPATHDIALOG_H
