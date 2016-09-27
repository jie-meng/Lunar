#include "recentprojectpathdialog.h"
#include <list>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "util/file.hpp"
#include "treeview.h"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui {

RecentProjectPathDialog::RecentProjectPathDialog(QWidget *parent) :
    QDialog(parent),
    plabel_recent_(NULL),
    ptree_view_(NULL),
    pnew_button_(NULL)
{
    init();
}

RecentProjectPathDialog::~RecentProjectPathDialog()
{

}

void RecentProjectPathDialog::init()
{
    plabel_recent_ = new QLabel();
    plabel_recent_->setText(tr("Recent project path"));

    QStringList header;
    header.append(tr("Name"));
    header.append(tr("Path"));
    ptree_view_ = new TreeView(header);

    pnew_button_ = new QPushButton();
    pnew_button_->setText(tr("New"));

    initGui();
    initConnections();
}

void RecentProjectPathDialog::initGui()
{
    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(plabel_recent_, 0, Qt::AlignLeft);
    pcenter_layout->addWidget(ptree_view_);
    pcenter_layout->addWidget(pnew_button_);

    list<string> ls;
    getRecentProjectPath(ls);
    for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it)
    {
        string name = fileBaseName(*it);
        QStringList qls;
        qls.append(StdStringToQString(name));
        qls.append(StdStringToQString(*it));
        ptree_view_->addItem(qls);
    }

    setLayout(pcenter_layout);

    setWindowTitle(tr("Reset project path"));

    ptree_view_->resizeColumnToContents(0);
    ptree_view_->resizeColumnToContents(1);
    setFixedWidth(ptree_view_->columnWidth(0) + ptree_view_->columnWidth(1));
    setFixedHeight(sizeHint().height());
}

void RecentProjectPathDialog::initConnections()
{
    connect(ptree_view_, SIGNAL(itemSelected(const QStringList&, int)),
            this, SLOT(onSelectRecentProjectPathItem(const QStringList&, int)));
    connect(pnew_button_, SIGNAL(clicked()), this, SLOT(onNewProjectPath()));
}

void RecentProjectPathDialog::onSelectRecentProjectPathItem(const QStringList& item, int number)
{
    Q_EMIT selectRecentProjectPath(item.at(1));
    close();
}

void RecentProjectPathDialog::onNewProjectPath()
{
    Q_EMIT newProjectPath();
    close();
}

} //namespace gui

