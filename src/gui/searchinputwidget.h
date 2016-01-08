#ifndef SEARCHINPUTWIDGET_H
#define SEARCHINPUTWIDGET_H

#include <QDialog>
#include "lunarcommon.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;

namespace gui
{

class SearchInputWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SearchInputWidget(const QString& dir,
                               const QString& text = tr(""),
                               const QString& file_filter = tr(""),
                               QWidget *parent = 0);
    ~SearchInputWidget();

signals:
    //dir, text, file_filter, case_sensitive, use_regex
    void inputOk(const QString&, const QString&, const QString&, bool, bool);
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
    QString dir_;
    QString text_;
    QString file_filter_;
    QLabel* plabel_;
    QLineEdit* pinput_text_;
    QLineEdit* pinput_file_filter_;
    QCheckBox* pcheck_case_sensitive_;
    QCheckBox* pcheck_use_regex_;
    QPushButton* pok_btn_;
    QPushButton* pcancel_btn_;
private:
    DISALLOW_COPY_AND_ASSIGN(SearchInputWidget)
};

}

#endif // SEARCHINPUTWIDGET_H
