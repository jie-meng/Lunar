#include "outputtext.h"
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QPalette>

namespace gui
{

OutputText::OutputText(QWidget *parent) :
    QTextEdit(parent)
{
    QPalette palette(this->palette());
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, QColor(Qt::white));
    palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(Qt::blue));

    this->setPalette(palette);
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
