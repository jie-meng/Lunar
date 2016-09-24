#include "recentprojectpathdialog.h"
#include <vector>
#include <list>
#include <QLabel>
#include <QVBoxLayout>
#include "util/file.hpp"
#include "treeview.h"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui {

RecentProjectPathDialog::RecentProjectPathDialog(QWidget *parent) :
    QDialog(parent),
    ptree_view_(NULL)
{
    init();
}

RecentProjectPathDialog::~RecentProjectPathDialog()
{

}

void RecentProjectPathDialog::init()
{
    initGui();
}

void RecentProjectPathDialog::initGui()
{
    QStringList header;
    header.append(tr("Name"));
    header.append(tr("Path"));
    ptree_view_ = new TreeView(header);
    connect(ptree_view_, SIGNAL(itemSelected(const QStringList&, int)),
            this, SLOT(onSelectRecentProjectPathItem(const QStringList&, int)));

    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(ptree_view_);

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

    setWindowTitle(tr("Select recent project path"));

    ptree_view_->resizeColumnToContents(0);
    ptree_view_->resizeColumnToContents(1);
    setFixedWidth(ptree_view_->columnWidth(0) + ptree_view_->columnWidth(1));
    setFixedHeight(sizeHint().height());
}

void RecentProjectPathDialog::onSelectRecentProjectPathItem(const QStringList& item, int number)
{
    Q_EMIT selectRecentProjectPath(item.at(1));
    close();
}

} //namespace gui

