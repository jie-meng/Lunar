#include "inputwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "util/base.hpp"

namespace gui {

InputWidget::InputWidget(const QString& label, QDialog *parent) :
    QDialog(parent),
    label_name_(label),
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
    pinput_line_ = new QLineEdit();
    pok_btn_ = new QPushButton("Ok");
    pcancel_btn_ = new QPushButton("Cancel");

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
    //connect(pinput_line_, SIGNAL(returnPressed()), this, SLOT(ok()));
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
