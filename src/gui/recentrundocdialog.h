#ifndef RECENTRUNDOCDIALOG_H
#define RECENTRUNDOCDIALOG_H

#include <QDialog>
#include "util/base.hpp"

class QLabel;

namespace gui
{

class TreeView;

class RecentRunDocDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecentRunDocDialog(QWidget *parent = 0);
    virtual ~RecentRunDocDialog();
signals:
    void runDoc(const QString&);
private:
    void init();
    void initConnections();
    void initGui();
private:
    TreeView* ptree_view_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentRunDocDialog)
};

} // namespace gui


#endif // RECENTRUNDOCDIALOG_H
