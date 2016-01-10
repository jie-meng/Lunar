## Lunar ##

A cross-platform script editor

- You can make your own auto-completion plugin as you wish.
- You can make your own goto-definition plugin as you wish.
- Currently provided: lua, cocos2dx_lua, python2 & 3, octave

### enviromment ###

- Qt5.5
- QScintilla 2.9.1

### build step ###

#### build luaexec ####

1. Get Util project from [git@github.com:joshua-meng/Util.git]()

2. Build and get an executable file: Util (Util.exe on windows).

3. Rename Util to luaexec.

4. luaexec is a lua script executor which contains basic lua (lua 5.2.3) & extensions (file, memory, net, process, regex, thread, csv, matrix) which Lunar needs.


#### build qscintilla ####

1. cd QScintilla-gpl-version_xxx\Qt4Qt5

2. qmake qscintilla.pro

3. make

4. make install

#### build Lunar ####

cd Lunar
luaexec remake_lunar.lua


### release ###

On all platform

- Make a dir named Lunar, lets' call it LUNAR_HOME. 
- and put all the files and folders in public to LUNAR_HOME.
- Put luaexec and Lunar to LUNAR_HOME.

On windows (You can find all the dlls in Qt installation directory):

- Put libstdc++-6.dll(mingw), libgcc_s_dw2-1.dll(mingw), libwinpthread-1.dll(mingw), Qt5Core.dll, Qt5Gui.dll,  Qt5PrintSupport.dll, Qt5Widgets.dll, qscintilla2.dll, icudt54, icuin54.dll, icuuc54.dll to LUNAR_HOME.
- Put qtaccessiblewidgets.dll to LUNAR_HOME/accessible
- Put qwindows.dll to LUNAR_HOME/platforms

### notes ###

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
