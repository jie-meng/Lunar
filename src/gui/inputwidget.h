#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QDialog>
#include "lunarcommon.h"

class QLabel;
class QLineEdit;
class QPushButton;

namespace gui {

class InputWidget : public QDialog
{
    Q_OBJECT
public:
    explicit InputWidget(const QString& label, const QString& text = tr(""), QDialog *parent = 0);

signals:
    void inputOk(const QString&);
public slots:

private slots:
    void ok();
    void cancel();
private:
    void init();
    void initFields();
    void initLayout();
    void initConnections();
private:
    QString label_name_;
    QString text_;
    QLabel* plabel_;
    QLineEdit* pinput_line_;
    QPushButton* pok_btn_;
    QPushButton* pcancel_btn_;
private:
    DISALLOW_COPY_AND_ASSIGN(InputWidget)
};

}

#endif // INPUTWIDGET_H
