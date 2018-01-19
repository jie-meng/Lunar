## Lunar ##

![](https://github.com/joshua-meng/Lunar/wiki/assets/lunar_linux.png)

A cross-platform (MacOS, Linux, Windows) script editor

- Customize your script api, auto-completion & goto-definition plugins.
- Currently supported: lua with extensions, cocos2dx_lua, cocos2dx_js, python2 & 3, cpp, octave 

### Preparations ###

- [Qt](https://www.qt.io/)
- [QScintilla](https://github.com/jie-meng/QScintilla)

Download Qt5 and QScintilla 2.10.

Install Qt, add Qt compiler directory to environment variable.

### Build steps ###

#### Build luaexec ####

1. Get Util project from [Util: Cross-platform C++ library](https://github.com/jie-meng/Util)

2. Run `python3 install_build_tool_here.py`
 
#### Build Qscintilla ####

1. cd QScintilla/Qt4Qt5

2. qmake qscintilla.pro

3. make

4. make install

#### Build Lunar ####

Run `./luaexec remake_lunar.lua`

### Release steps ###

On Linux and Windows 

- Make a dir named Lunar, lets' call it LUNAR_HOME. 
- Put all the files and folders in publish to LUNAR_HOME.
- Put luaexec and Lunar to LUNAR_HOME.

On windows extras (You can find all the dlls in Qt installation directory):

- Put libstdc++-6.dll(mingw), libgcc_s_dw2-1.dll(mingw), libwinpthread-1.dll(mingw), Qt5Core.dll, Qt5Gui.dll,  Qt5PrintSupport.dll, Qt5Widgets.dll, qscintilla2_qt5.dll, icudt54, icuin54.dll, icuuc54.dll to LUNAR_HOME.
- Put qtaccessiblewidgets.dll to LUNAR_HOME/accessible (No need now)
- Put qwindows.dll to LUNAR_HOME/platforms

On MacOS

- Run `./luaexec deploy_on_mac.lua` on terminal to make Lunar.dmg (Follow command tips to give correct information)
- Install Lunar from Lunar.dmg to `/Applications`
- Add `export PATH=$PATH:/Applications/Lunar.app/Contents/MacOS` to your environment. (Then you can start Lunar from your terminal in current directory by typing command 'luna')

Final step of all platforms

- Open Lunar from command line. (For Linux and Windows, input `Lunar` then press `Enter`. for MacOS input `luna` then press `Enter`)

- Open File Explorer with shortcut key `CTRL (COMMAND on MacOS) + SHIFT + E`, then `CTRL (COMMAND on MacOS) + J` open Context Menu. Select `Tools/lunar/update_lunar_plugins` to update packages (apis / assets / plugins / tools / luaexeclib / extension.lua) to the latest. (Make sure you have svn command line installed).

- Enjoy it!

[Download Link](https://github.com/jie-meng/Lunar/releases)

### Notes ###

- 2018.01.18

    V1.9.2 Fix cannot edit and save lunar.cfg in Lunar bug. Support goto Lunar settings.

- 2018.01.12

    V1.9.1 Support plugins and tools auto update: Use tools/update_lunar_plugins.lua

- 2018.01.11

    V1.9.0 Update Qscintilla version to 2.10.2; python2/3 plugins make greatly enhancement. (use pydoc_gen.lua in apis/python to generate api first)

- 2017.11.14
    
    V1.8.8 Support locate current file in file explorer.

- 2017.11.12
    
    V1.8.7 Support find files.
    
- 2017.11.08
	
	V1.8.6 Support start from MAC terminal with command 'luna'

- 2017.08.17

	V1.8.3 Support run recent script with F6.
    
- 2017.05.21
    
    V1.8.1 Support cocos2dx-js.
    
- 2017.03.01

    V1.7.7 Update QScintilla2 to 2.10. Support Markdown & JSON.

- 2016.12.29
	
    V1.7.2 Update util to V1.0.5. All util extend api reformed.
    
- 2016.6.28
    
    V1.5.8 Support cpp plugins.

- 2016.05.20

    V1.5.3 Support MacOS. 
    
- 2016.01.10
    
    Support goto_definition, jump list.

    You can make your goto definition plugin for your own needs. Use plugins/goto_lua.lua as an example.
   
- 2016.01.08

	Support extension_tools, right-click mouse on directory of file explore, you can use extension tools.
	
	You can make your tools of you own. The extension_tools can be programmed in any script language you Lunar supported.
    
- 2015.05.15

	Support more than 36 types of languages.
	
	Avs, Bash, Batch, CMake, CoffeeSript, CPP, CSharp, CSS, D, Diff, Fortran, Fortran77, Html, Idl, 
	Java, JavaScript, Lua, Makefile, Matlab, Octave, Pascal, Perl, Po, PostScript, Pov, Properties, 
	Python, Ruby, Spice, Sql, Tcl, Tex, Verilog, Vhdl, Xml, Yaml.
	
	Modify extension.lua as the example of lua can easily support a new language.
	
	You also need to manage language apis in apis directory.

	If you want better programming experience of auto-completion. Try make a plugin follow the lua example in plugins.

- 2015.03.10

	Support octave .m file. Set the path of octave cmdline application to the lunar.cfg parameter "Run.Runner.Octave", then you can run octave files in Lunar.
