#include "aboutdialog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace gui
{

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
    plabel_email_(NULL),
    plabel_github_(NULL),
    plabel_wiki_(NULL),
    plabel_version_(NULL),
    plabel_platform_(NULL)
{
    init();
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::init()
{
    initGui();
}

void AboutDialog::initGui()
{
    plabel_version_ = new QLabel(tr("Version: 1.9.9"));

    plabel_email_ = new QLabel(tr("Email: jmengxy@gmail.com"));

    plabel_github_ = new QLabel();
    plabel_github_->setText("<a href=\"https://github.com/jie-meng/Lunar\">Lunar on Github</a>");
    plabel_github_->setTextFormat(Qt::RichText);
    plabel_github_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    plabel_github_->setOpenExternalLinks(true);

    plabel_wiki_ = new QLabel();
    plabel_wiki_->setText("<a href=\"https://github.com/jie-meng/Lunar/wiki\">Lunar Wiki</a>");
    plabel_wiki_->setTextFormat(Qt::RichText);
    plabel_wiki_->setTextInteractionFlags(Qt::TextBrowserInteraction);
    plabel_wiki_->setOpenExternalLinks(true);

    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(plabel_version_);
	pcenter_layout->addWidget(plabel_email_);
	pcenter_layout->addWidget(plabel_github_);
    pcenter_layout->addWidget(plabel_wiki_);

    setLayout(pcenter_layout);

    setWindowTitle(tr("Information"));
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());
}

} // namespace gui

