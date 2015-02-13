#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabBar>
#include <QTabWidget>
#include "finddialog.h"

namespace gui
{

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent),
    ptab_widget_(NULL),
    pcase_check_box_(NULL),
    pbackward_checkbox_(NULL),
    pwholeword_checkbox_(NULL),
    pwrap_around_checkbox_(NULL),
    pfind_tab_(NULL),
    preplace_tab_(NULL),
    first_find_(true),
    find_text_(tr(""))
{
    Init();
}

FindDialog::~FindDialog()
{
}

void FindDialog::Init()
{
    InitGui();
    InitConnections();
}

void FindDialog::InitGui()
{
    ptab_widget_ = new QTabWidget;
    pfind_tab_ = new FindTab;
    preplace_tab_ = new ReplaceTab;
    ptab_widget_->addTab(pfind_tab_, "Find");
    ptab_widget_->addTab(preplace_tab_, "Replace");

    pcase_check_box_ = new QCheckBox(tr("&Case Sensitive"));
    pbackward_checkbox_ = new QCheckBox(tr("Search &backward"));
    pwholeword_checkbox_ = new QCheckBox(tr("&Whole word"));
    pwrap_around_checkbox_ = new QCheckBox(tr("Wra&p around"));
    pwrap_around_checkbox_->setChecked(true);
    pfind_in_output_checkbox_ = new QCheckBox(tr("Find in &output"));

    QVBoxLayout* pmain_layout = new QVBoxLayout;
    pmain_layout->addWidget(ptab_widget_);
    pmain_layout->addWidget(pcase_check_box_);
    pmain_layout->addWidget(pbackward_checkbox_);
    pmain_layout->addWidget(pwholeword_checkbox_);
    pmain_layout->addWidget(pwrap_around_checkbox_);
    pmain_layout->addWidget(pfind_in_output_checkbox_);
    setLayout(pmain_layout);

    setWindowTitle(tr("Find"));
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());
}

void FindDialog::InitConnections()
{
    //self
    connect(pcase_check_box_, SIGNAL(clicked()), this, SLOT(OptionsChanged()));
    connect(pbackward_checkbox_, SIGNAL(clicked()), this, SLOT(OptionsChanged()));
    connect(pwholeword_checkbox_, SIGNAL(clicked()), this, SLOT(OptionsChanged()));
    connect(pwrap_around_checkbox_, SIGNAL(clicked()), this, SLOT(OptionsChanged()));
    connect(ptab_widget_, SIGNAL(currentChanged(int)), this, SLOT(TabChanged(int)));

    //findtab
    connect(pfind_tab_, SIGNAL(OptionsChanged()), this, SLOT(OptionsChanged()));
    connect(pfind_tab_, SIGNAL(CmdFind(const QString&)), this, SLOT(CmdFind(const QString&)));
    connect(pfind_tab_, SIGNAL(FindTextChangeSignal(const QString&)), this, SLOT(TabFindTextChanged(const QString&)));
    //replacetab
    connect(preplace_tab_, SIGNAL(OptionsChanged()), this, SLOT(OptionsChanged()));
    connect(preplace_tab_, SIGNAL(CmdFind(const QString&)), this, SLOT(CmdFind(const QString&)));
    connect(preplace_tab_, SIGNAL(CmdReplace(const QString&)), this, SLOT(CmdReplace(const QString&)));
    connect(preplace_tab_, SIGNAL(CmdReplaceAll(const QString&, const QString&)), this, SLOT(CmdReplaceAll(const QString&, const QString&)));
    connect(preplace_tab_, SIGNAL(FindTextChangeSignal(const QString&)), this, SLOT(TabFindTextChanged(const QString&)));
}

void FindDialog::CmdFind(const QString& text)
{
    Qt::CaseSensitivity cs = pcase_check_box_->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool whole_word = pwholeword_checkbox_->isChecked() ? true : false;
    bool find_previous = pbackward_checkbox_->isChecked() ? true : false;
    bool wrap = pwrap_around_checkbox_->isChecked() ? true : false;
    bool find_in_output = pfind_in_output_checkbox_->isChecked() ? true : false;

    Q_EMIT Find(text, first_find_, cs, find_previous, whole_word, wrap, find_in_output);
    //make a flag
    first_find_ = false;
}

void FindDialog::closeEvent(QCloseEvent* e)
{
    QWidget::closeEvent(e);

    OptionsChanged();
}

void FindDialog::OptionsChanged()
{
    first_find_ = true;
}

void FindDialog::CmdReplace(const QString& replace_with_text)
{
    bool find_in_output = pfind_in_output_checkbox_->isChecked() ? true : false;

    Q_EMIT Replace(replace_with_text, find_in_output);
}

void FindDialog::CmdReplaceAll(const QString& text, const QString& replace_with_text)
{
    Qt::CaseSensitivity cs = pcase_check_box_->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool whole_word = pwholeword_checkbox_->isChecked() ? true : false;
    bool find_previous = pbackward_checkbox_->isChecked() ? true : false;
    bool find_in_output = pfind_in_output_checkbox_->isChecked() ? true : false;

    Q_EMIT ReplaceAll(text, replace_with_text, cs, find_previous, whole_word, find_in_output);

    OptionsChanged();
}

void FindDialog::TabChanged(int index)
{
    switch (index)
    {
        case TabFind:
            pfind_tab_->SetFindText(find_text_);
            setWindowTitle(tr("Find"));
            break;

        case TabReplace:
            preplace_tab_->SetFindText(find_text_);
            setWindowTitle(tr("Replace"));
            break;
        default:
            break;
    }
}

void FindDialog::TabFindTextChanged(const QString& text)
{
    find_text_ = text;
}

////////////////////////////////////////////////////
// class name : FindTab
// description :
// author :
// time : 2012-01-10-18.13
////////////////////////////////////////////////////
FindTab::FindTab(QWidget *parent)
    : QWidget(parent)
{
    Init();
}

FindTab::~FindTab()
{

}

void FindTab::Init()
{
    InitGui();
    InitConnections();
}
void FindTab::InitGui()
{
    plabel_find_ = new QLabel(tr("Find &what:"));
    pfind_edit_ = new QLineEdit;
    plabel_find_->setBuddy(pfind_edit_);

    pfind_button_ = new QPushButton(tr("&Find"));
    pfind_button_->setDefault(true);
    pfind_button_->setEnabled(false);

    QHBoxLayout* topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(plabel_find_);
    topLeftLayout->addWidget(pfind_edit_);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    //leftLayout->addStretch();

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(pfind_button_);
    //rightLayout->addStretch();

    QHBoxLayout* centerLayout = new QHBoxLayout;
    centerLayout->addLayout(leftLayout);
    centerLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);
}

void FindTab::InitConnections()
{
    connect(pfind_edit_, SIGNAL(textChanged(const QString&)), this, SIGNAL(FindTextChangeSignal(const QString&)));
    connect(pfind_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(FindTextChanged(const QString&)));
    connect(pfind_button_, SIGNAL(clicked()), this, SLOT(FindClicked()));
}

void FindTab::FindTextChanged(const QString &text)
{
    pfind_button_->setEnabled(!text.isEmpty());
    Q_EMIT OptionsChanged();
}

void FindTab::FindClicked()
{
    Q_EMIT CmdFind(pfind_edit_->text());
}

////////////////////////////////////////////////////
// class name : ReplaceTab
// description :
// author :
// time : 2012-01-10-19.57
////////////////////////////////////////////////////
ReplaceTab::ReplaceTab(QWidget *parent)
    : QWidget(parent)
{
    Init();
}

ReplaceTab::~ReplaceTab()
{

}

void ReplaceTab::Init()
{
    InitGui();
    InitConnections();
}
void ReplaceTab::InitGui()
{
    plabel_find_ = new QLabel(tr("Find &what:"));
    pfind_edit_ = new QLineEdit;
    plabel_find_->setBuddy(pfind_edit_);

    plabel_replace_ = new QLabel(tr("Rep&lace with"));
    preplace_edit_ = new QLineEdit;
    plabel_replace_->setBuddy(preplace_edit_);

    pfind_button_ = new QPushButton(tr("&Find"));
    pfind_button_->setDefault(true);
    pfind_button_->setEnabled(false);

    preplace_button_ = new QPushButton(tr("&Replace"));
    preplace_button_->setDefault(true);
    preplace_button_->setEnabled(false);

    preplace_all_button_ = new QPushButton(tr("Replace &all"));
    preplace_all_button_->setDefault(true);
    preplace_all_button_->setEnabled(false);

    QHBoxLayout* topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(plabel_find_);
    topLeftLayout->addWidget(pfind_edit_);

    QHBoxLayout* topLeftLayout1 = new QHBoxLayout;
    topLeftLayout1->addWidget(plabel_replace_);
    topLeftLayout1->addWidget(preplace_edit_);

    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addLayout(topLeftLayout1);
    leftLayout->addStretch();

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(pfind_button_);
    rightLayout->addWidget(preplace_button_);
    rightLayout->addWidget(preplace_all_button_);
    //rightLayout->addStretch();

    QHBoxLayout* centerLayout = new QHBoxLayout;
    centerLayout->addLayout(leftLayout);
    centerLayout->addLayout(rightLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(centerLayout);
    setLayout(mainLayout);
}

void ReplaceTab::InitConnections()
{
    connect(pfind_edit_, SIGNAL(textChanged(const QString&)), this, SIGNAL(FindTextChangeSignal(const QString&)));
    connect(pfind_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
    connect(preplace_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
    connect(pfind_button_, SIGNAL(clicked()), this, SLOT(FindClicked()));
    connect(preplace_button_, SIGNAL(clicked()), this, SLOT(ReplaceClicked()));
    connect(preplace_all_button_, SIGNAL(clicked()), this, SLOT(ReplaceAllClicked()));
}

void ReplaceTab::TextChanged()
{
    pfind_button_->setEnabled(!pfind_edit_->text().isEmpty());
    preplace_button_->setEnabled(!pfind_edit_->text().isEmpty());
    preplace_all_button_->setEnabled(!pfind_edit_->text().isEmpty());
    Q_EMIT OptionsChanged();
}

void ReplaceTab::FindClicked()
{
    Q_EMIT CmdFind(pfind_edit_->text());
}

void ReplaceTab::ReplaceClicked()
{
    Q_EMIT CmdReplace(preplace_edit_->text());
    Q_EMIT CmdFind(pfind_edit_->text());
}

void ReplaceTab::ReplaceAllClicked()
{
    Q_EMIT CmdReplaceAll(pfind_edit_->text(), preplace_edit_->text());
}

} // namespace gui
