######################################################################
# Automatically generated by qmake (3.0) Wed Mar 4 11:13:53 2015
######################################################################

TEMPLATE = app
TARGET = Lunar
INCLUDEPATH += .

# Input
HEADERS += src/luaexecutor.h \
           src/lunarcommon.h \
           src/processmsgthread.h \
           src/gui/aboutdialog.h \
           src/gui/apiloader.h \
           src/gui/dockwidgetex.h \
           src/gui/docview.h \
           src/gui/finddialog.h \
           src/gui/maintabwidget.h \
           src/gui/mainwindow.h \
           src/gui/outputtext.h \
           src/gui/outputwidget.h \
           src/util/base.hpp \
           src/util/cfg.hpp \
           src/util/collection.hpp \
           src/util/file.hpp \
           src/util/lexicalcast.hpp \
           src/util/net.hpp \
           src/util/process.hpp \
           src/util/regex.hpp \
           src/util/string.hpp \
           src/util/thread.hpp \
           src/util/time.hpp \
           src/util/base/base.hpp \
           src/util/base/baseclass.hpp \
           src/util/base/in.hpp \
           src/util/base/memory.hpp \
           src/util/base/out.hpp \
           src/util/regex/deelx.h
SOURCES += src/luaexecutor.cpp \
           src/lunarcommon.cpp \
           src/main.cpp \
           src/processmsgthread.cpp \
           src/gui/aboutdialog.cpp \
           src/gui/apiloader.cpp \
           src/gui/dockwidgetex.cpp \
           src/gui/docview.cpp \
           src/gui/finddialog.cpp \
           src/gui/maintabwidget.cpp \
           src/gui/mainwindow.cpp \
           src/gui/outputtext.cpp \
           src/gui/outputwidget.cpp \
           src/util/cfg.cpp \
           src/util/file.cpp \
           src/util/lexicalcast.cpp \
           src/util/net.cpp \
           src/util/process.cpp \
           src/util/regex.cpp \
           src/util/string.cpp \
           src/util/thread.cpp \
           src/util/time.cpp \
           src/util/base/out.cpp
RESOURCES += Lunar.qrc

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += _PLATFORM_WINDOWS_ \
			_GUI_

INCLUDEPATH +=  src

LIBS += -lws2_32 \
		-lShlwapi \
        -lqscintilla2

RC_FILE += Lunar.rc
