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
    QTreeWidget(parent),
    loaded_(false)
{
    init();
}

void FileExplorerWidget::loadFilesIfFirstTime()
{
    if (!loaded_)
    {
        reloadFiles();
        loaded_ = true;
    }
}

void FileExplorerWidget::init()
{
    //init gui
    setColumnCount(1);
    setHeaderLabels(QStringList(""));

    initConnections();
}

void FileExplorerWidget::initConnections()
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));
}

void FileExplorerWidget::reloadFiles()
{
    clear();

    QList<QTreeWidgetItem*> items;
    QTreeWidgetItem* proot_item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(currentPath()).second)));
    proot_item->setIcon(0, QIcon(tr(":/res/open.png")));
    loadPathFilesRecursively(currentPath(), proot_item);
    items.append(proot_item);

    insertTopLevelItems(0, items);
}

void FileExplorerWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        onItemReturn(currentItem(), currentColumn());
        break;
    case Qt::Key_Delete:
        onDeleteItems(currentItem(), currentColumn());
        break;
    default:
        break;
    }

    QTreeWidget::keyPressEvent(event);
}

void FileExplorerWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = new QMenu();
    QAction *act_refresh = menu->addAction(tr("Refresh"));
    menu->addSeparator();
    QAction *act_delete = menu->addAction(tr("Delete"));
    connect(act_refresh, SIGNAL(triggered()),this,SLOT(reloadFiles()));
    connect(act_delete, SIGNAL(triggered()),this,SLOT(deleteCurrentItem()));
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
            Q_EMIT openFile(StdStringToQString(fullpathname));
    }
}

void FileExplorerWidget::onItemReturn(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

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
            Q_EMIT openFile(StdStringToQString(fullpathname));
    }
    else
    {
        item->setExpanded(!item->isExpanded());
    }
}

QString FileExplorerWidget::getCurrentSelectedDir()
{
    QTreeWidgetItem* item = currentItem();
    if (!item)
        return StdStringToQString(currentPath());

    QString filename = item->text(currentColumn());
    QTreeWidgetItem* pnode = item->parent();
    while(pnode)
    {
        filename = pnode->text(currentColumn()) + "/" + filename;
        pnode = pnode->parent();
    }

    string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);
    if (isPathDir(fullpathname))
        return StdStringToQString(fullpathname);
    else if (isPathFile(fullpathname))
        return StdStringToQString(splitPathname(fullpathname).first);
    else
        return StdStringToQString(currentPath());
}

void FileExplorerWidget::deleteCurrentItem()
{
    onDeleteItems(currentItem(), currentColumn());
}

void FileExplorerWidget::onDeleteItems(QTreeWidgetItem* item, int column)
{
    //cannot delete root node
    if (!item || !item->parent())
        return;

    QString filename = item->text(column);
    QTreeWidgetItem* pnode = item->parent();
    while(pnode)
    {
        filename = pnode->text(column) + "/" + filename;
        pnode = pnode->parent();
    }

    string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);

    if (item->childCount() == 0)
    {
        if (QMessageBox::No == QMessageBox::question(this, "question", "Are you sure to delete " + filename + "?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
            return;
        }
        else
        {
            pathRemove(fullpathname);
            QTreeWidgetItem* pparent = item->parent();
            if (pparent)
            {
                pparent->removeChild(item);
                delete item;
            }
        }
    }
    else
    {
        if (QMessageBox::No == QMessageBox::question(this, "question", "Are you sure to delete " + filename + "?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
            return;
        }
        else
        {
            pathRemoveAll(fullpathname);
            QTreeWidgetItem* pparent = item->parent();
            if (pparent)
            {
                pparent->removeChild(item);
                delete item;
            }
        }
    }
}

void FileExplorerWidget::onFileSaved(const QString& file)
{
    QTreeWidgetItem* pnode = findDirItemWithFile(file);
    if (!pnode)
        return;

    string path = QStringToStdString(file);
    std::pair<string, string> path_name = splitPathname(path);

    for (int i=0; i<pnode->childCount(); ++i)
    {
        //already has
        if (QStringToStdString(pnode->child(i)->text(0)) == path_name.second)
            return;
    }

    QTreeWidgetItem* pchild = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(path_name.second)));
    pchild->setIcon(0, QIcon(tr(":/res/new.png")));
    pnode->addChild(pchild);
    pnode->sortChildren(0, Qt::AscendingOrder);
}

void FileExplorerWidget::onAllFilesSaved()
{
    reloadFiles();
}

QTreeWidgetItem* FileExplorerWidget::findDirItemWithFile(const QString& file)
{
    string path = QStringToStdString(file);
    std::pair<string, string> path_name = splitPathname(path);
    if (!strContains(path_name.first, currentPath()))
        return NULL;

    QTreeWidgetItem* proot = topLevelItem(0);
    if (proot == NULL)
        return NULL;

    string relative_dir = strReplace(path_name.first, currentPath() + "/", "");
    vector<string> vec;
    strSplit(relative_dir, "/", vec);
    QTreeWidgetItem* pnode = proot;
    for (int i=0; i<(int)vec.size(); ++i)
    {
        if (pnode != NULL)
        {
            bool found = false;
            for (int j=0; j<pnode->childCount(); ++j)
            {
                if (QStringToStdString(pnode->child(j)->text(0)) == vec[i])
                {
                    pnode = pnode->child(j);
                    found = true;
                }
            }

            if (!found)
                return NULL;
        }
    }

    return pnode;
}

}
