# Lunar #
A cross-platform Lua script extension and IDE

## introduction ## 
A Lua script extension and IDE
* Luaexec: Lua script basic(lua 5.2.3) & extensions (file, memory, net, process, regex, thread)
* Lunar: Lua IDE

## enviromment ##
* Qt5.3 (mingw 4.8.2)
* QScintilla 2.8

## build step ##
* build qscintilla
cd QScintilla-gpl-2.8.4\Qt4Qt5
qmake qscintilla.pro
make
make install

* build Lunar
cd Lunar
luaexec qt_remake.lua


## release ##

* luaexec.exe must run with libstdc++-6.dll, libgcc_s_dw2-1.dll of mingw 4.8 
* Lunar.exe must run with libstdc++-6.dll(mingw4.8), libgcc_s_dw2-1.dll(mingw4.8), Qt5Core.dll, Qt5Gui.dll, qscintilla2.dll.
