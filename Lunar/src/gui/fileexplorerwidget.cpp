#include "fileexplorerwidget.h"
#include <algorithm>
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
#include "inputwidget.h"

using namespace std;
using namespace util;

namespace gui
{

FileExplorerWidget::FileExplorerWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    init();
}

FileExplorerWidget::~FileExplorerWidget()
{
    clear();
}

void FileExplorerWidget::init()
{
    //init gui
    setColumnCount(1);
    setHeaderLabels(QStringList(""));

    initConnections();
    loadRoot();
}

void FileExplorerWidget::initConnections()
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));
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
    QAction *act_new_folder = menu->addAction(tr("New Folder"));
    QAction *act_delete = menu->addAction(tr("Delete"));

    connect(act_refresh, SIGNAL(triggered()),this,SLOT(loadRoot()));
    connect(act_new_folder, SIGNAL(triggered()), this, SLOT(newFolder()));
    connect(act_delete, SIGNAL(triggered()),this,SLOT(deleteCurrentItem()));

    menu->exec(e->globalPos());
    delete menu;

    QTreeWidget::contextMenuEvent(e);
}

void FileExplorerWidget::loadRoot()
{
    clear();

    QList<QTreeWidgetItem*> items;
    QTreeWidgetItem* proot_item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(currentPath()).second)));
    proot_item->setIcon(0, QIcon(tr(":/res/open.png")));
    loadNode(proot_item);
    items.append(proot_item);

    insertTopLevelItems(0, items);
}

bool FileExplorerWidget::loadNode(QTreeWidgetItem* item)
{
    QString path = getNodeAbsolutePath(item);
    if (path.length() == 0)
        return false;
    string str_path = QStringToStdString(path);

    if (isPathDir(str_path))
    {
        //delete all
        while (item->childCount() > 0)
        {
            QTreeWidgetItem* child = item->child(0);
            item->removeChild(child);
            delete child;
        }

        //reload dir nodes
        vector<string> vec;
        DirFilter dir_filter;
        listFiles(str_path, vec, &dir_filter);
        sort(vec.begin(), vec.end());        
        vector<string>::iterator it;
        for (it = vec.begin(); it != vec.end(); ++it)
        {
            QTreeWidgetItem* new_child = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(*it).second)));
            new_child->setIcon(0, QIcon(tr(":/res/open.png")));
            item->addChild(new_child);
        }

        //reload file nodes
        vec.clear();
        FileFilter file_filter;
        listFiles(str_path, vec, &file_filter);
        sort(vec.begin(), vec.end());        
        for (it = vec.begin(); it != vec.end(); ++it)
        {
            QTreeWidgetItem* new_child = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(*it).second)));
            new_child->setIcon(0, QIcon(tr(":/res/new.png")));
            item->addChild(new_child);
        }

        return true;
    }
    else
    {
        return false;
    }
}

QString FileExplorerWidget::getNodeAbsolutePath(QTreeWidgetItem* item)
{
    if (NULL == item)
        return tr("");

    QString filename = item->text(0);
    QTreeWidgetItem* pnode = item->parent();
    while(pnode)
    {
        filename = pnode->text(0) + "/" + filename;
        pnode = pnode->parent();
    }
    string fullpathname = splitPathname(currentPath()).first + "/" + QStringToStdString(filename);
    return StdStringToQString(fullpathname);
}

QString FileExplorerWidget::getNodeRelativePath(QTreeWidgetItem* item)
{
    string str = QStringToStdString(getNodeAbsolutePath(item));
    if (str.empty())
        return tr("");

    return StdStringToQString(strReplace(str, currentPath(), splitPathname(currentPath()).second));
}

void FileExplorerWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

    if (item->childCount() == 0)
    {
        //if it's dir, just reload and return
        if (loadNode(item))
        {
            //DoubleClicked is different from return key press, strange.
            return;
        }

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
        loadNode(item);
        //DoubleClicked is different from return key press, strange.
    }
}

void FileExplorerWidget::onItemReturn(QTreeWidgetItem *item, int column)
{
    if (!item)
        return;

    if (item->childCount() == 0)
    {
        //if it's dir, just reload and return
        if (loadNode(item))
        {
            item->setExpanded(true);
            return;
        }

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
        loadNode(item);
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

void FileExplorerWidget::newFolder()
{
    QString path = getNodeAbsolutePath(currentItem());
    if (isPathDir(QStringToStdString(path)))
    {
        InputWidget inputwidget(QString(tr("Create Folder in ")) + getNodeRelativePath(currentItem()));
        connect(&inputwidget, SIGNAL(inputOk(const QString&)), this, SLOT(newFolderOk(const QString&)));
        inputwidget.exec();
    }
}

void FileExplorerWidget::newFolderOk(const QString& folder_name)
{
    if (folder_name.length() == 0)
    {
        LunarMsgBox(strFormat("Error: Folder name is empty"));
        return;
    }

    QString path = getNodeAbsolutePath(currentItem());
    if (mkDir(QStringToStdString(path + "/" + folder_name)))
        loadNode(currentItem());
    else
        LunarMsgBox(strFormat("Error: Folder %s already exists in %s.", QStringToStdString(folder_name).c_str(), QStringToStdString(path).c_str()));
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
    QTreeWidgetItem* pnode = findDirNodeItemWithFile(file);
    loadNode(pnode);
    if (pnode)
        pnode->setExpanded(true);
}

void FileExplorerWidget::onAllFilesSaved()
{
    loadRoot();
}

QTreeWidgetItem* FileExplorerWidget::findDirNodeItemWithFile(const QString& file)
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
