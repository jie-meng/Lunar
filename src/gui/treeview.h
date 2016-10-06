#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeWidget>
#include "util/base.hpp"

class QKeyEvent;

namespace gui {

class TreeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit TreeView(const QStringList& header, QWidget *parent = 0);

signals:
    void itemSelected(const QStringList&, int);
    void itemDeleted(const QStringList&, int);
public slots:
    void addItem(const QStringList& item_list);
protected:
    virtual void keyPressEvent(QKeyEvent *event);
private slots:
    void onItemSelect(QTreeWidgetItem *item, int column);
    void onItemDelete(QTreeWidgetItem *item, int column);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
private:
    void initConnections();
private:
    DISALLOW_COPY_AND_ASSIGN(TreeView)
};

} // namespace gui

#endif // TREEVIEW_H
