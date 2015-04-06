#include "outputwidget.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "lunarcommon.h"
#include "outputtext.h"

namespace gui
{

using namespace util;

OutputWidget::OutputWidget(QWidget* parent) :
    QWidget(parent),
    poutput_(NULL),
    pinput_line_(NULL),
    pinput_btn_(NULL)
{
    //ctor
    init();
}

OutputWidget::~OutputWidget()
{
    //dtor
}

void OutputWidget::append(const QString& str)
{
    Synchronize sc(mutex_);
    //poutput_->append(str);
    poutput_->moveCursor(QTextCursor::End);
    poutput_->insertPlainText(str);
}

QString OutputWidget::getOutput()
{
    Synchronize sc(mutex_);
    return poutput_->toPlainText();
}

void OutputWidget::clear()
{
    Synchronize sc(mutex_);
    poutput_->clear();
}

void OutputWidget::init()
{
    initFields();
    initLayout();
    initConnections();
}

void OutputWidget::initFields()
{
    poutput_ = new OutputText();
    pinput_line_ = new QLineEdit();
    pinput_btn_ = new QPushButton("input");

    poutput_->setReadOnly(true);
}

void OutputWidget::initLayout()
{
     //bottom
    QHBoxLayout* pbottom_layout = new QHBoxLayout();
    pbottom_layout->addWidget(pinput_line_);
    pbottom_layout->addWidget(pinput_btn_);

    //main
    QVBoxLayout* pmain_layout = new QVBoxLayout();
    pmain_layout->addWidget(poutput_);
    pmain_layout->addLayout(pbottom_layout);

    this->setLayout(pmain_layout);
}

void OutputWidget::initConnections()
{
    connect(pinput_btn_, SIGNAL(clicked()), this, SLOT(input()));
    connect(pinput_line_, SIGNAL(returnPressed()), this, SLOT(input()));
}

void OutputWidget::input()
{
    emit sendInput(pinput_line_->text() + "\n");
    pinput_line_->clear();
}

bool OutputWidget::findText(const QString& expr,
                           bool re,
                           bool cs,
                           bool wo,
                           bool wrap,
                           bool forward,
                           bool first_find,
                           bool from_start
                           )
{
    QTextDocument::FindFlags ff = 0;
    if (cs)
        ff |= QTextDocument::FindCaseSensitively;
    if (wo)
        ff |= QTextDocument::FindWholeWords;
    if (!forward)
        ff |= QTextDocument::FindBackward;

    Synchronize sc(mutex_);
    return poutput_->find(expr, ff);
}

} //namespace gui
