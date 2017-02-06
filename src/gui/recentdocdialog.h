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
    void init();
    void initConnections();
    void initGui();
signals:
    void selectDoc(const QString&);
public slots:
    void onSelectRecentDocItem(const QStringList& item, int number);
private:
    TreeView* ptree_view_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentDocDialog)
};

} // namespace gui


#endif // RECENTDOCDIALOG_H
