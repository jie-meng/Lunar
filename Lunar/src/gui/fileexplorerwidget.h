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

signals:
    void openFile(const QString&);
public slots:

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *e);
private:
    void init();
    void initConnections();
    void loadPathFilesRecursively(const std::string& path, QTreeWidgetItem* pparent);
private Q_SLOTS:
    void reloadFiles();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onItemReturn(QTreeWidgetItem *item, int column);
   // void onDeleteItems(QTreeWidgetItem* item, int column);
private:
    QList<QTreeWidgetItem*> items_;
};

}


#endif // FILEEXPLORERWIDGET_H
