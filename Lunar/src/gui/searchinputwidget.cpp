#include "searchinputwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "util/file.hpp"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui
{

SearchInputWidget::SearchInputWidget(
                                    const QString& dir,
                                     const QString& text,
                                     const QString& file_filter,
                                     QWidget *parent) :
    QDialog(parent),
    dir_(dir),
    text_(text),
    file_filter_(file_filter),
    pinput_text_(NULL),
    pinput_file_filter_(NULL),
    pcheck_case_sensitive_(NULL),
    pcheck_use_regex_(NULL),
    pok_btn_(NULL),
    pcancel_btn_(NULL)
{
    init();
}

SearchInputWidget::~SearchInputWidget()
{

}

void SearchInputWidget::init()
{
    initFields();
    initLayout();
    initConnections();
}

void SearchInputWidget::initFields()
{
    plabel_ = new QLabel(QString(tr("Search files in ")) + dir_);
    pinput_text_ = new QLineEdit(text_);
    pinput_file_filter_ = new QLineEdit(file_filter_);
    pcheck_case_sensitive_ = new QCheckBox(tr("Case sensitive"));
    pcheck_case_sensitive_->setChecked(true);
    pcheck_use_regex_ = new QCheckBox(tr("Use regexp"));
    pok_btn_ = new QPushButton("Ok");
    pcancel_btn_ = new QPushButton("Cancel");

    if (pinput_text_->text().length() != 0)
    {
        string name = QStringToStdString(pinput_text_->text());
        name = fileBaseName(name);
        pinput_text_->setSelection(0, name.length());
    }

    pok_btn_->setDefault(true);
}

void SearchInputWidget::initLayout()
{
    //text
    QHBoxLayout* ptext_layout = new QHBoxLayout();
    ptext_layout->addWidget(new QLabel(tr("Text")));
    ptext_layout->addWidget(pinput_text_);

    //file filter
    QHBoxLayout* pfile_filter_layout = new QHBoxLayout();
    pfile_filter_layout->addWidget(new QLabel("File Filter"));
    pfile_filter_layout->addWidget(pinput_file_filter_);

     //bottom
    QHBoxLayout* pbottom_layout = new QHBoxLayout();
    pbottom_layout->addWidget(pok_btn_);
    pbottom_layout->addWidget(pcancel_btn_);

    //main
    QVBoxLayout* pmain_layout = new QVBoxLayout();
    pmain_layout->addWidget(plabel_);
    pmain_layout->addLayout(ptext_layout);
    pmain_layout->addLayout(pfile_filter_layout);
    pmain_layout->addWidget(pcheck_case_sensitive_);
    pmain_layout->addWidget(pcheck_use_regex_);
    pmain_layout->addLayout(pbottom_layout);

    this->setLayout(pmain_layout);
}

void SearchInputWidget::initConnections()
{
    connect(pok_btn_, SIGNAL(clicked()), this, SLOT(ok()));
    connect(pcancel_btn_, SIGNAL(clicked()), this, SLOT(cancel()));
}

void SearchInputWidget::ok()
{
    Q_EMIT inputOk(
                   dir_,
                   pinput_text_->text(),
                   pinput_file_filter_->text(),
                   pcheck_case_sensitive_->isChecked(),
                   pcheck_use_regex_->isChecked());
    close();
}

void SearchInputWidget::cancel()
{
    close();
}

}

