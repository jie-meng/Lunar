#include "fileexplorerwidget.h"
#include <vector>
#include <QTreeWidgetItem>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include "util/file.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

FileExplorerWidget::FileExplorerWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    init();
    reloadFiles();
}

void FileExplorerWidget::init()
{
    //init gui
    setColumnCount(1);
    setHeaderLabels(QStringList(""));
    //setContextMenuPolicy(Qt::ActionsContextMenu);

    initConnections();
}

void FileExplorerWidget::initConnections()
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));
}

void FileExplorerWidget::reloadFiles()
{
    clear();
    items_.clear();
    QTreeWidgetItem* proot = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(currentPath()).second)));
    proot->setIcon(0, QIcon(tr(":/res/open.png")));
    loadPathFilesRecursively(currentPath(), proot);
    items_.append(proot);
    insertTopLevelItems(0, items_);
}

void FileExplorerWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Enter:
        onItemReturn(currentItem(), currentColumn());
        break;
    case Qt::Key_Delete:

        break;
    default:
        break;
    }

    QTreeWidget::keyPressEvent(event);
}

void FileExplorerWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = new QMenu();
    QAction *act = menu->addAction(tr("Refresh"));
    connect(act,SIGNAL(triggered()),this,SLOT(reloadFiles()));
    menu->exec(e->globalPos());
    delete menu;

    QTreeWidget::contextMenuEvent(e);
}

void FileExplorerWidget::loadPathFilesRecursively(const std::string& path, QTreeWidgetItem* pparent)
{
    vector<string> vec;
    DirFilter dir_filter;
    listFiles(path, vec, &dir_filter);
    vector<string>::iterator it;
    for (it = vec.begin(); it != vec.end(); ++it)
    {
        QTreeWidgetItem* pitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(*it).second)));
        pitem->setIcon(0, QIcon(tr(":/res/open.png")));
        loadPathFilesRecursively(*it, pitem);
        pparent->addChild(pitem);
    }

    vec.clear();
    FileFilter file_filter;
    listFiles(path, vec, &file_filter);
    for (it = vec.begin(); it != vec.end(); ++it)
    {
        QTreeWidgetItem* pitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(*it).second)));
         pitem->setIcon(0, QIcon(tr(":/res/new.png")));
        pparent->addChild(pitem);
    }
}

void FileExplorerWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item->childCount() == 0)
    {
        QString filename = item->text(column);
        QTreeWidgetItem* pnode = item->parent();
        while(pnode)
        {
            filename = pnode->text(column) + "/" + filename;
            pnode = pnode->parent();
        }

        string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);
        if (isPathFile(fullpathname))
            emit openFile(StdStringToQString(fullpathname));
    }
}

void FileExplorerWidget::onItemReturn(QTreeWidgetItem *item, int column)
{
    if (item->childCount() == 0)
    {
        QString filename = item->text(column);
        QTreeWidgetItem* pnode = item->parent();
        while(pnode)
        {
            filename = pnode->text(column) + "/" + filename;
            pnode = pnode->parent();
        }

        string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);
        if (isPathFile(fullpathname))
            emit openFile(StdStringToQString(fullpathname));
    }
    else
    {
        item->setExpanded(!item->isExpanded());
    }
}

//void FileExplorerWidget::onDeleteItems(QTreeWidgetItem* item, int column)
//{
//    //cannot delete root node
//    if (!item->parent())
//        return;

//    QString filename = item->text(column);
//    QTreeWidgetItem* pnode = item->parent();
//    while(pnode)
//    {
//        filename = pnode->text(column) + "/" + filename;
//        pnode = pnode->parent();
//    }

//    string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);

//    if (item->childCount() == 0)
//    {
//        if (QMessageBox::No == QMessageBox::question(this, "question", "Are you sure to delete this?",
//                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
//        {
//            return;
//        }
//        else
//        {
//            pathRemove(fullpathname);
//        }
//    }
//    else
//    {
//        if (QMessageBox::No == QMessageBox::question(this, "question", "Are you sure to delete this?",
//                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
//        {
//            return;
//        }
//        else
//        {
//            pathRemoveAll(fullpathname);
//        }
//    }
//}

}
