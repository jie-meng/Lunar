#ifndef GUI_DOCKWIDGETEX_H
#define GUI_DOCKWIDGETEX_H

#include <QDockWidget>
#include "util/base.hpp"

namespace gui
{

class DockWidgetEx : public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWidgetEx(const QString &title, QWidget* parent);
    virtual ~DockWidgetEx();
Q_SIGNALS:
    void onClose();
protected:
    void closeEvent(QCloseEvent* event);
private:
    DISALLOW_COPY_AND_ASSIGN(DockWidgetEx)
};

} // namespace gui

#endif // GUI_DOCKWIDGETEX_H
