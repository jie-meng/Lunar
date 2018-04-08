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
    plabel_version_ = new QLabel(tr("Version: 1.10.0"));
    plabel_email_ = new QLabel(tr("Email: jmengxy@gmail.com"));
    plabel_github_ = createLinkLabel("Lunar on Github", "https://github.com/jie-meng/Lunar");
    plabel_wiki_ = createLinkLabel("Lunar Wiki", "https://github.com/jie-meng/Lunar/wiki");

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

QLabel* AboutDialog::createLinkLabel(const QString& name, const QString& link)
{
    QLabel* label = new QLabel();
    label->setText(QString("<a href=\"%2\">%1</a>").arg(name, link));
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);

    return label;
}

} // namespace gui

