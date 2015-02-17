#include <QApplication>
#include <QIcon>
#include <vector>
#include "util/cfg.hpp"
#include "gui/mainwindow.h"
#include "lunarcommon.h"

int main(int argc, char* argv[])
{
    InitLunarCommon(argc, argv);

    QApplication a(argc, argv);
    gui::MainWindow main_window;
    if (!main_window.Init())
        return 0;

    main_window.resize(LunarGlobal::get_mainwindow_width(), LunarGlobal::get_mainwindow_height());
    main_window.show();

    a.setWindowIcon(QIcon("res/app.ico"));

    return a.exec();
}
