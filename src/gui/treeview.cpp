#include "treeview.h"
#include <QKeyEvent>
#include "lunarcommon.h"

namespace gui {

TreeView::TreeView(const QStringList& header, QWidget *parent) : QTreeWidget(parent)
{
    setHeaderLabels(header);
    initConnections();
}

void TreeView::addItem(const QStringList& item_list)
{
    addTopLevelItem(new QTreeWidgetItem((QTreeWidget*)0, item_list));
}

void TreeView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        onItemSelect(currentItem(), currentColumn());
        break;
    default:
        break;
    }

    QTreeWidget::keyPressEvent(event);
}

void TreeView::onItemSelect(QTreeWidgetItem *item, int column)
{
    if (!item || column < 0)
        return;

    QStringList item_list;
    for (int i = 0; i<columnCount(); ++i)
        item_list.append(item->text(i));

    Q_EMIT itemSelected(item_list, column);
}

void TreeView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    onItemSelect(item, column);
}

void TreeView::initConnections()
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));
}

}

