#ifndef RECENTDOCDIALOG_H
#define RECENTDOCDIALOG_H

#include <QDialog>
#include "util/base.hpp"

class QLabel;

namespace gui
{

class TreeView;

class RecentDocDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecentDocDialog(QWidget *parent = 0);
    virtual ~RecentDocDialog();
signals:
    void selectDoc(const QString&);
private:
    void init();
    void initConnections();
    void initGui();
private:
    TreeView* ptree_view_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentDocDialog)
};

} // namespace gui


#endif // RECENTDOCDIALOG_H
