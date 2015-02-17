#ifndef GUI_OUTPUTWIDGET_H
#define GUI_OUTPUTWIDGET_H

#include <QtCore/QString>
#include <QWidget>
#include "util/thread.hpp"

class QTextEdit;
class QLineEdit;
class QPushButton;

namespace gui
{

class OutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OutputWidget(QWidget* parent = 0);
    virtual ~OutputWidget();
    void append(const QString& str);
    QString getOutput();
    void clear();
    bool IsOutputActive();
    bool FindText(const QString& expr,
                               bool re,
                               bool cs,
                               bool wo,
                               bool wrap,
                               bool forward,
                               bool first_find,
                               bool from_start = false
                               );
Q_SIGNALS:
    void sendInput(const QString&);
public Q_SLOTS:
    void input();
private:
    void init();
    void initFields();
    void initLayout();
    void initConnections();
private:
    QTextEdit* poutput_;
    QLineEdit* pinput_line_;
    QPushButton* pinput_btn_;
    util::Mutex mutex_;
private:
    DISALLOW_COPY_AND_ASSIGN(OutputWidget)
};

} //namespace gui

#endif // GUI_OUTPUTWIDGET_H
