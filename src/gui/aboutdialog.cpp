#include "aboutdialog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace gui
{

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
    plabel_email_(NULL),
    plabel_version_(NULL),
    plabel_platform_(NULL)
{
    init();
}

AboutDialog::~AboutDialog()
{}

void AboutDialog::init()
{
    initGui();
}

void AboutDialog::initGui()
{
    plabel_version_ = new QLabel(tr("Version: 1.9.5"));
    plabel_email_ = new QLabel(tr("Email: jmengxy@gmail.com"));

    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(plabel_version_);
	pcenter_layout->addWidget(plabel_email_);

    setLayout(pcenter_layout);

    setWindowTitle(tr("Information"));
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());
}

} // namespace gui

