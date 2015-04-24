######################################################################
# Automatically generated by qmake (3.0) Fri Apr 24 15:41:18 2015
######################################################################

TEMPLATE = app
TARGET = Lunar
INCLUDEPATH += .

# Input
HEADERS += src/extension.h \
           src/filetype.h \
           src/luaexecutor.h \
           src/lunarcommon.h \
           src/gui/aboutdialog.h \
           src/gui/apiloader.h \
           src/gui/dockwidgetex.h \
           src/gui/docview.h \
           src/gui/fileexplorerwidget.h \
           src/gui/finddialog.h \
           src/gui/inputwidget.h \
           src/gui/lunarapi.h \
           src/gui/maintabwidget.h \
           src/gui/mainwindow.h \
           src/gui/outputtext.h \
           src/gui/outputwidget.h \
           src/gui/qsciapisex.h \
           src/util/any.hpp \
           src/util/base.hpp \
           src/util/cfg.hpp \
           src/util/cmdline.hpp \
           src/util/collection.hpp \
           src/util/file.hpp \
           src/util/lexicalcast.hpp \
           src/util/luaextend.hpp \
           src/util/luax.hpp \
           src/util/net.hpp \
           src/util/process.hpp \
           src/util/regex.hpp \
           src/util/thread.hpp \
           src/util/typeinfo.hpp \
           src/util/base/base.hpp \
           src/util/base/baseclass.hpp \
           src/util/base/constants.hpp \
           src/util/base/in.hpp \
           src/util/base/memory.hpp \
           src/util/base/out.hpp \
           src/util/base/string.hpp \
           src/util/base/time.hpp \
           src/util/regex/deelx.h \
           src/util/lua/extend/lfilelib.hpp \
           src/util/lua/extend/lregexlib.hpp \
           src/util/lua/src/lapi.h \
           src/util/lua/src/lauxlib.h \
           src/util/lua/src/lcode.h \
           src/util/lua/src/lctype.h \
           src/util/lua/src/ldebug.h \
           src/util/lua/src/ldo.h \
           src/util/lua/src/lfunc.h \
           src/util/lua/src/lgc.h \
           src/util/lua/src/llex.h \
           src/util/lua/src/llimits.h \
           src/util/lua/src/lmem.h \
           src/util/lua/src/lobject.h \
           src/util/lua/src/lopcodes.h \
           src/util/lua/src/lparser.h \
           src/util/lua/src/lprefix.h \
           src/util/lua/src/lstate.h \
           src/util/lua/src/lstring.h \
           src/util/lua/src/ltable.h \
           src/util/lua/src/ltm.h \
           src/util/lua/src/lua.h \
           src/util/lua/src/lua.hpp \
           src/util/lua/src/luaconf.h \
           src/util/lua/src/lualib.h \
           src/util/lua/src/lundump.h \
           src/util/lua/src/lvm.h \
           src/util/lua/src/lzio.h
SOURCES += src/extension.cpp \
           src/luaexecutor.cpp \
           src/lunarcommon.cpp \
           src/main.cpp \
           src/gui/aboutdialog.cpp \
           src/gui/apiloader.cpp \
           src/gui/dockwidgetex.cpp \
           src/gui/docview.cpp \
           src/gui/fileexplorerwidget.cpp \
           src/gui/finddialog.cpp \
           src/gui/inputwidget.cpp \
           src/gui/lunarapi.cpp \
           src/gui/maintabwidget.cpp \
           src/gui/mainwindow.cpp \
           src/gui/outputtext.cpp \
           src/gui/outputwidget.cpp \
           src/gui/qsciapisex.cpp \
           src/util/cfg.cpp \
           src/util/cmdline.cpp \
           src/util/file.cpp \
           src/util/lexicalcast.cpp \
           src/util/luaextend.cpp \
           src/util/luax.cpp \
           src/util/net.cpp \
           src/util/process.cpp \
           src/util/regex.cpp \
           src/util/thread.cpp \
           src/util/base/out.cpp \
           src/util/base/string.cpp \
           src/util/base/time.cpp \
           src/util/lua/extend/lfilelib.cpp \
           src/util/lua/extend/lregexlib.cpp \
           src/util/lua/src/lapi.c \
           src/util/lua/src/lauxlib.c \
           src/util/lua/src/lbaselib.c \
           src/util/lua/src/lbitlib.c \
           src/util/lua/src/lcode.c \
           src/util/lua/src/lcorolib.c \
           src/util/lua/src/lctype.c \
           src/util/lua/src/ldblib.c \
           src/util/lua/src/ldebug.c \
           src/util/lua/src/ldo.c \
           src/util/lua/src/ldump.c \
           src/util/lua/src/lfunc.c \
           src/util/lua/src/lgc.c \
           src/util/lua/src/linit.c \
           src/util/lua/src/liolib.c \
           src/util/lua/src/llex.c \
           src/util/lua/src/lmathlib.c \
           src/util/lua/src/lmem.c \
           src/util/lua/src/loadlib.c \
           src/util/lua/src/lobject.c \
           src/util/lua/src/lopcodes.c \
           src/util/lua/src/loslib.c \
           src/util/lua/src/lparser.c \
           src/util/lua/src/lstate.c \
           src/util/lua/src/lstring.c \
           src/util/lua/src/lstrlib.c \
           src/util/lua/src/ltable.c \
           src/util/lua/src/ltablib.c \
           src/util/lua/src/ltm.c \
           src/util/lua/src/lua.c \
           src/util/lua/src/luac.c \
           src/util/lua/src/lundump.c \
           src/util/lua/src/lutf8lib.c \
           src/util/lua/src/lvm.c \
           src/util/lua/src/lzio.c
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