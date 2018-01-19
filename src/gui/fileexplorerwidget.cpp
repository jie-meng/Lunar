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
    setHeaderLabels(QStringList(tr("File Explorer")));

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
    showContextMenu(e->globalPos());
    QTreeWidget::contextMenuEvent(e);
}

void FileExplorerWidget::onClickExtensionTool(const std::string& str)
{
    emit executeExtensionTool(StdStringToQString(str), getCurrentSelectedDir(), tr(""));
}

void FileExplorerWidget::loadRoot()
{
    clear();

    QList<QTreeWidgetItem*> items;
    QTreeWidgetItem* proot_item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(currentPath()).second)));
    proot_item->setIcon(0, QIcon(tr(":/res/fe_folder.png")));
    loadNode(proot_item);
    items.append(proot_item);

    insertTopLevelItems(0, items);
    Q_EMIT widthChanged(200);
}

void FileExplorerWidget::locateFile(const QString& file)
{
    string path = QStringToStdString(file);
    std::pair<string, string> path_name = splitPathname(path);
    if (!strContains(path_name.first, currentPath()))
        return;

    QTreeWidgetItem* proot = topLevelItem(0);
    if (proot == NULL)
        return;

    string relative_dir = strReplace(path_name.first, currentPath(), "");
    if (relative_dir.length() == 0)
    {
        //file in root direct children
        proot->setExpanded(true);
        for (int i=0; i<proot->childCount(); ++i)
        {
            if (QStringToStdString(proot->child(i)->text(0)) == path_name.second)
            {    
                scrollToItem(proot->child(i));
                setCurrentItem(proot->child(i));
            }
        }
        return;
    }

    if (strStartWith(relative_dir, "/"))
        relative_dir = strRight(relative_dir, relative_dir.length()-1);

    vector<string> vec;
    strSplit(relative_dir, "/", vec);
    QTreeWidgetItem* pnode = proot;
    proot->setExpanded(true);
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
                    if (pnode->childCount() == 0)
                        onItemReturn(pnode, currentColumn());
                    pnode->setExpanded(true);
                    
                    found = true;
                    break;
                }
            }

            if (found)
                continue;
            else
                break;
        }
        else
        {
            break;
        }
    }
    
    if (pnode != NULL && pnode->childCount() > 0)
    {
        for (int i=0; i<pnode->childCount(); ++i)
        {
            if (QStringToStdString(pnode->child(i)->text(0)) == path_name.second)
            {    
                scrollToItem(pnode->child(i));
                setCurrentItem(pnode->child(i));
            }
        }
    }
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
            new_child->setIcon(0, QIcon(tr(":/res/fe_folder.png")));
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
            new_child->setIcon(0, QIcon(tr(":/res/fe_file.png")));
            item->addChild(new_child);
        }

        return true;
    }
    else
    {
        return false;
    }
}


bool FileExplorerWidget::loadNodeFiles(QTreeWidgetItem* item)
{
    QString path = getNodeAbsolutePath(item);
    if (path.length() == 0)
        return false;

    string str_path = QStringToStdString(path);

    if (isPathDir(str_path))
    {
        //delete all file nodes, leave folder nodes there
        while (item->childCount() > 0)
        {
            QTreeWidgetItem* child = item->child(item->childCount()-1);
            string child_path = QStringToStdString(getNodeAbsolutePath(child));
            if (!isPathDir(child_path))
            {
                item->removeChild(child);
                delete child;
            }
            else
            {
                break;
            }
        }

        //reload file nodes
        vector<string> vec;
        FileFilter file_filter;
        listFiles(str_path, vec, &file_filter);
        sort(vec.begin(), vec.end());
        vector<string>::iterator it;
        for (it = vec.begin(); it != vec.end(); ++it)
        {
            QTreeWidgetItem* new_child = new QTreeWidgetItem((QTreeWidget*)0, QStringList(StdStringToQString(splitPathname(*it).second)));
            new_child->setIcon(0, QIcon(tr(":/res/fe_file.png")));
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
            Q_EMIT widthChanged(0);
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
    Q_EMIT widthChanged(0);
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
            this->resizeColumnToContents(currentColumn());
            Q_EMIT widthChanged(this->columnWidth(currentColumn()));

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

    this->resizeColumnToContents(currentColumn());
    Q_EMIT widthChanged(this->columnWidth(currentColumn()));
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

void FileExplorerWidget::setAsProjectPath()
{
    QString path = getNodeAbsolutePath(currentItem());
    if (isPathDir(QStringToStdString(path)))
    {
        emit setProjectPath(path);
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
        LunarMsgBox(strFormat("Error: Folder \"%s\" already exists in %s.", QStringToStdString(folder_name).c_str(), QStringToStdString(path).c_str()));
}

void FileExplorerWidget::deleteCurrentItem()
{
    onDeleteItems(currentItem(), currentColumn());
}

void FileExplorerWidget::renameCurrentItem()
{
    QString path = getNodeAbsolutePath(currentItem());
    InputWidget inputwidget(QString(tr("Rename ") + currentItem()->text(0) + " to"),
                            currentItem()->text(0));
    connect(&inputwidget, SIGNAL(inputOk(const QString&)), this, SLOT(renameCurrentItemOk(const QString&)));
    inputwidget.exec();
}

void FileExplorerWidget::renameCurrentItemOk(const QString& new_name)
{
    string str_name = QStringToStdString(new_name);
    string str_from_pathname = QStringToStdString(getNodeAbsolutePath(currentItem()));
    string str_to_pathname = splitPathname(str_from_pathname).first + "/" + str_name;
    if (pathRename(str_from_pathname, str_to_pathname))
    {
        if (isPathDir(str_to_pathname))
        {
            currentItem()->setText(0, StdStringToQString(fileBaseName(str_to_pathname)));
            emit renameDir(StdStringToQString(str_from_pathname), StdStringToQString(str_to_pathname));
        }
        else
        {
            QTreeWidgetItem* parent = currentItem()->parent();
            loadNodeFiles(parent);
            emit renameFile(StdStringToQString(str_from_pathname), StdStringToQString(str_to_pathname));
        }
    }
    else
    {
        LunarMsgBox("Error: Rename failed.");
    }
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
    if (isPathDir(fullpathname))
    {
        if (QMessageBox::No == QMessageBox::question(this, "question", "Are you sure to delete " + filename + "?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
            return;
        

        //emit this signal before remove file, because slot hoster would check the files in dir
        emit removeDir(StdStringToQString(fullpathname));

        pathRemoveAll(fullpathname);
        item->parent()->removeChild(item);
        delete item;
    }
    else
    {
        if (QMessageBox::No == QMessageBox::question(this, "", "Are you sure to delete " + filename + "?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
            return;

        if (pathRemove(fullpathname))
        {
            QTreeWidgetItem* pparent = item->parent();
            pparent->removeChild(item);
            delete item;
            emit removeFile(StdStringToQString(fullpathname));
        }
    }
}

void FileExplorerWidget::onFileSaved(const QString& file)
{
    QTreeWidgetItem* pdir = findDirNodeItemWithFile(file);
    if (pdir)
    {
        vector<string> current_dir_node_files;
        for (int i = 0; i<pdir->childCount(); ++i)
        {
            auto child = pdir->child(i);
            current_dir_node_files.push_back(QStringToStdString(getNodeAbsolutePath(child)));
        }

        auto str_dir = QStringToStdString(getNodeAbsolutePath(pdir));
        vector<string> current_dir_files;
        listFiles(str_dir, current_dir_files, 0);

        sort(current_dir_files.begin(), current_dir_files.end());
        sort(current_dir_node_files.begin(), current_dir_node_files.end());

        if (current_dir_files != current_dir_node_files)
            loadNodeFiles(pdir);
    }
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

    string relative_dir = strReplace(path_name.first, currentPath(), "");
    if (relative_dir.length() == 0)
        return proot;

    if (strStartWith(relative_dir, "/"))
        relative_dir = strRight(relative_dir, relative_dir.length()-1);

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
                    break;
                }
            }

            if (found)
                continue;
            else
                return NULL;
        }
        else
        {
            return NULL;
        }
    }

    return pnode;
}

void FileExplorerWidget::showContextMenu()
{
    showContextMenu(mapToGlobal(pos()));
}

void FileExplorerWidget::showContextMenu(const QPoint &pos)
{
    QMenu* menu = new QMenu();

    QAction* act_refresh = menu->addAction(tr("Refresh"));
    QAction* act_set_path = menu->addAction(tr("Set as project path"));
    menu->addSeparator();
    QAction* act_new_folder = menu->addAction(tr("New folder"));
    QAction* act_rename = menu->addAction(tr("Rename"));
    QAction* act_delete = menu->addAction(tr("Delete"));

    string extension_tools_path = LunarGlobal::getInstance().getAppPath() + "/" + LunarGlobal::getInstance().getExtensionToolsPath();
    if (isPathDir(extension_tools_path))
    {
        showExtensionTools(menu->addMenu("Extension Tools"), extension_tools_path);
    }

    connect(act_refresh, SIGNAL(triggered()), this, SLOT(loadRoot()));
    connect(act_set_path, SIGNAL(triggered()), this, SLOT(setAsProjectPath()));
    connect(act_new_folder, SIGNAL(triggered()), this, SLOT(newFolder()));
    connect(act_rename, SIGNAL(triggered()), this, SLOT(renameCurrentItem()));
    connect(act_delete, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));

    menu->exec(pos);
    delete menu;
}

void FileExplorerWidget::showExtensionTools(QMenu* pmenu, const std::string& dst_dir)
{
    DirFilter df;
    vector<string> dirs;
    if (listFiles(dst_dir, dirs, &df) > 0)
    {
        sort(dirs.begin(), dirs.end());
        for (vector<string>::iterator it = dirs.begin(); it != dirs.end(); ++it)
        {
            QMenu* psubmenu = pmenu->addMenu(StdStringToQString(fileBaseName(*it)));
            showExtensionTools(psubmenu, *it);
        }
    }
    
    FileFilter ff;
    vector<string> files;
    if (listFiles(dst_dir, files, &ff) > 0)
    {
        sort(files.begin(), files.end());
        for (vector<string>::iterator it = files.begin(); it != files.end(); ++it)
        {
            QAction* act = pmenu->addAction(StdStringToQString(fileBaseName(*it)));
            connect(act, &QAction::triggered, UtilBind(&FileExplorerWidget::onClickExtensionTool, this, *it));
        }
    }
}

}
