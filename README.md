# Lunar #
A cross-platform Lua script extension and IDE
  

* Luaexec: Lua script basic(lua 5.2.3) & extensions (file, memory, net, process, regex, thread, csv, matrix)
* Lunar: Lua & Octave IDE

-------------------------------------------------


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

## notes ##

2015.03.10

Now Lunar support octave .m file. Set the path of octave cmdline application to the cfg parameter "Run.Runner.Octave", then you can run octave files in Lunar.

2015.05.15

Now Lunar can support more than 36 types of languages.

Avs, Bash, Batch, CMake, CoffeeSript, CPP, CSharp, CSS, D, Diff, Fortran, Fortran77, Html, Idl, 
Java, JavaScript, Lua, Makefile, Matlab, Octave, Pascal, Perl, Po, PostScript, Pov, Properties, 
Python, Ruby, Spice, Sql, Tcl, Tex, Verilog, Vhdl, Xml, Yaml.

Modify extension.lua as the example of lua can easily support a new language.

You also need to manage language apis in apis directory.

If you want better programming experience of auto-completion. Try make a plugin follow the lua example in plugins.
