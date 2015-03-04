#ifndef OUTPUTTEXT_H
#define OUTPUTTEXT_H

#include <QtCore/QObject>
#include <QTextEdit>

class QContextMenuEvent;

namespace gui
{

class OutputText : public QTextEdit
{
    Q_OBJECT
public:
    explicit OutputText(QWidget *parent = 0);

signals:

public slots:
    void onClear();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
};

}

#endif // OUTPUTTEXT_H
