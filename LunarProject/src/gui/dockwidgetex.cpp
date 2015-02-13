#include "dockwidgetex.h"
#include <QMessageBox.h>

namespace gui
{

DockWidgetEx::DockWidgetEx(const QString &title, QWidget* parent) :
    QDockWidget(title, parent, 0)
{
    //ctor
}

DockWidgetEx::~DockWidgetEx()
{
    //dtor
}

void DockWidgetEx::closeEvent(QCloseEvent* event)
{
    emit onClose();

    QDockWidget::closeEvent(event);
}

} // namespace gui
