#include "recentdocdialog.h"
#include <list>
#include <QVBoxLayout>
#include "util/file.hpp"
#include "treeview.h"
#include "lunarcommon.h"

using namespace std;
using namespace util;

namespace gui {

RecentDocDialog::RecentDocDialog(QWidget* parent) :
    QDialog(parent),
    ptree_view_(NULL)
{
    init();
}

RecentDocDialog::~RecentDocDialog()
{
}

void RecentDocDialog::init()
{
    initGui();
    initConnections();
}

void RecentDocDialog::initGui()
{
    QStringList header;
    header.append(tr("Name"));
    header.append(tr("Path"));
    ptree_view_ = new TreeView(header);

    LunarGlobal::getInstance().trimRecentDocs();
    auto it = LunarGlobal::getInstance().recentDocsIterator();
    while (it.hasNext())
    {
        string file = it.next();
        auto path_name = splitPathname(file);
        QStringList qls;
        qls.append(StdStringToQString(path_name.second));
        qls.append(StdStringToQString(file));
        ptree_view_->addItem(qls);
    }

    QVBoxLayout* pcenter_layout = new QVBoxLayout;
    pcenter_layout->addWidget(ptree_view_);
    setLayout(pcenter_layout);

    setWindowTitle(tr("Recent documents"));

    ptree_view_->resizeColumnToContents(0);
    ptree_view_->resizeColumnToContents(1);
    setFixedWidth(ptree_view_->columnWidth(0) + ptree_view_->columnWidth(1));
    setFixedHeight(sizeHint().height());
}

void RecentDocDialog::initConnections()
{
    connect(ptree_view_, &TreeView::itemSelected, [this](const QStringList& item, int number)
    {
        Q_EMIT selectDoc(item.at(1));
        close();
    });

    connect(ptree_view_, &TreeView::itemDeleted, [this](const QStringList& item, int number)
    {
        LunarGlobal::getInstance().removeRecentDoc(QStringToStdString(item.at(1)));        
    });
}

} //namespace gui
