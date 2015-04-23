#ifndef FILEEXPLORERWIDGET_H
#define FILEEXPLORERWIDGET_H

#include <string>
#include <QList>
#include <QTreeWidget>

class QTreeWidgetItem;
class QContextMenuEvent;

namespace gui
{

class FileExplorerWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit FileExplorerWidget(QWidget *parent = 0);
    QString getCurrentSelectedDir();
    void loadFilesIfFirstTime();
signals:
    void openFile(const QString&);
public slots:
    void onFileSaved(const QString& file);
    void onAllFilesSaved();
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *e);
private:
    void init();
    void initConnections();
    void loadPathFilesRecursively(const std::string& path, QTreeWidgetItem* pparent);
private Q_SLOTS:
    void reloadFiles();
    void deleteCurrentItem();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemReturn(QTreeWidgetItem *item, int column);
    void onDeleteItems(QTreeWidgetItem* item, int column);
    QTreeWidgetItem* findDirItemWithFile(const QString& file);
private:
    bool loaded_;
};

}


#endif // FILEEXPLORERWIDGET_H
