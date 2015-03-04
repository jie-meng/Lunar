#include "outputtext.h"
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

namespace gui
{

OutputText::OutputText(QWidget *parent) :
    QTextEdit(parent)
{
}

void OutputText::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = createStandardContextMenu();
    menu->addSeparator();
    QAction *act = menu->addAction(tr("Clear"));
    connect(act,SIGNAL(triggered()),this,SLOT(onClear()));
    menu->exec(e->globalPos());
    delete menu;
}

void OutputText::onClear()
{
    clear();
}

}
