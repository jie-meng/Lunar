## Lunar ##

![](https://github.com/joshua-meng/Lunar/wiki/assets/lunar_linux.png)

A cross-platform (MacOS, Linux, Windows) script editor

- Customize your script api, auto-completion & goto-definition plugins.
- Currently supported: lua with extensions, cocos2dx_lua, python2 & 3, cpp, octave 

### Preparations ###

- [Qt](https://www.qt.io/)
- [QScintilla](https://www.riverbankcomputing.com/software/qscintilla/download)

Download Qt5 and QScintilla 2.9.

Install Qt, add Qt compiler directory to environment variable.

### Build step ###

#### Build luaexec ####

1. Get Util project from [Util: Cross-platform C++ library](https://github.com/joshua-meng/Util)

2. Build luaexec follow the README.MD of Util. 

3. luaexec is a lua script executor which contains basic lua & extensions (file, memory, net, process, regex, thread, csv, matrix) which Lunar needs.

#### Build Qscintilla ####

1. cd QScintilla-gpl-version_xxx\Qt4Qt5

2. qmake qscintilla.pro

3. make

4. make install

#### Build Lunar ####

luaexec remake_lunar.lua

### Release ###

On Linux and Windows 

- Make a dir named Lunar, lets' call it LUNAR_HOME. 
- Put all the files and folders in publish to LUNAR_HOME.
- Put luaexec and Lunar to LUNAR_HOME.

On windows extras (You can find all the dlls in Qt installation directory):

- Put libstdc++-6.dll(mingw), libgcc_s_dw2-1.dll(mingw), libwinpthread-1.dll(mingw), Qt5Core.dll, Qt5Gui.dll,  Qt5PrintSupport.dll, Qt5Widgets.dll, qscintilla2.dll, icudt54, icuin54.dll, icuuc54.dll to LUNAR_HOME.
- Put qtaccessiblewidgets.dll to LUNAR_HOME/accessible
- Put qwindows.dll to LUNAR_HOME/platforms

On MacOS

1. Put luaexec to /usr/local/bin

2. Use command "otool -L Lunar.app/Contents/MacOS/Lunar" to check the library dependency

3. Use command "install_name_tool -change libqscintilla2.12.dylib(found in result of otool command) {absolute_path_of_libqscintilla2.12.dylib} Lunar.app/Contents/MacOS/Lunar" to change the load path of special dylibs

4. Use command "macdeployqt Lunar.app"

5. Put all the files and folders in publish to Lunar.app/Contents/MacOS

6. Put Lunar.app to /Applications/
(Run postbuild_mac.lua can do steps 2~5 automatically)

### Notes ###

- 2015.03.10

	Support octave .m file. Set the path of octave cmdline application to the cfg parameter "Run.Runner.Octave", then you can run octave files in Lunar.

- 2015.05.15

	Support more than 36 types of languages.
	
	Avs, Bash, Batch, CMake, CoffeeSript, CPP, CSharp, CSS, D, Diff, Fortran, Fortran77, Html, Idl, 
	Java, JavaScript, Lua, Makefile, Matlab, Octave, Pascal, Perl, Po, PostScript, Pov, Properties, 
	Python, Ruby, Spice, Sql, Tcl, Tex, Verilog, Vhdl, Xml, Yaml.
	
	Modify extension.lua as the example of lua can easily support a new language.
	
	You also need to manage language apis in apis directory.

	If you want better programming experience of auto-completion. Try make a plugin follow the lua example in plugins.

- 2016.01.08

	Support extension_tools, right-click mouse on directory of file explore, you can use extension tools.
	
	You can make your extension_tools of you own. The extension_tools can be programmed in any script language you Lunar supported.

- 2016.01.10
    
    Support goto_definition, jump list.

    You can make your goto definition plugin for your own needs. Use plugins/goto_lua.lua as an example.

- 2016.05.20

    V1.5.3 support MacOS.

- 2016.6.28
    
    V1.5.8 support cpp plugins.
