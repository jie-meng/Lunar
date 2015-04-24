#include "inputwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "util/file.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui {

InputWidget::InputWidget(const QString& label, const QString& text, QDialog *parent) :
    QDialog(parent),
    label_name_(label),
    text_(text),
    pinput_line_(NULL),
    pok_btn_(NULL),
    pcancel_btn_(NULL)
{
    init();
}

void InputWidget::init()
{
    initFields();
    initLayout();
    initConnections();
}

void InputWidget::initFields()
{
    plabel_ = new QLabel(label_name_);
    pinput_line_ = new QLineEdit(text_);
    pok_btn_ = new QPushButton("Ok");
    pcancel_btn_ = new QPushButton("Cancel");

    if (pinput_line_->text().length() != 0)
    {
        string name = QStringToStdString(pinput_line_->text());
        name = fileBaseName(name);
        pinput_line_->setSelection(0, name.length());
    }

    pok_btn_->setDefault(true);
}

void InputWidget::initLayout()
{
     //bottom
    QHBoxLayout* pbottom_layout = new QHBoxLayout();
    pbottom_layout->addWidget(pok_btn_);
    pbottom_layout->addWidget(pcancel_btn_);

    //main
    QVBoxLayout* pmain_layout = new QVBoxLayout();
    pmain_layout->addWidget(plabel_);
    pmain_layout->addWidget(pinput_line_);
    pmain_layout->addLayout(pbottom_layout);

    this->setLayout(pmain_layout);
}

void InputWidget::initConnections()
{
    connect(pok_btn_, SIGNAL(clicked()), this, SLOT(ok()));
    connect(pcancel_btn_, SIGNAL(clicked()), this, SLOT(cancel()));
}

void InputWidget::ok()
{
    Q_EMIT inputOk(pinput_line_->text());
    close();
}

void InputWidget::cancel()
{
    close();
}

}
