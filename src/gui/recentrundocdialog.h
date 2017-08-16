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
    void init();
    void initConnections();
    void initGui();
signals:
    void runDoc(const QString&);
public slots:
    void onSelectRecentDocItem(const QStringList& item, int number);
private:
    TreeView* ptree_view_;
private:
    DISALLOW_COPY_AND_ASSIGN(RecentRunDocDialog)
};

} // namespace gui


#endif // RECENTRUNDOCDIALOG_H
