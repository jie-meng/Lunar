## Lunar ##

![](https://github.com/joshua-meng/Lunar/wiki/assets/lunar_linux.png)

A cross-platform (MacOS, Linux, Windows) script editor

- Customize your script api, auto-completion & goto-definition plugins.
- Currently supported: lua, python, cpp, octave, cocos2dx_lua, cocos2dx_js

### Preparations ###

- [Qt](https://www.qt.io/)
- [QScintilla](https://github.com/jie-meng/QScintilla)

Download Qt5 and QScintilla 2.10.

Install Qt, add Qt compiler directory to environment variable.

### Build steps ###

#### Build luaexec ####

1. Get Util project from [Util: Cross-platform C++ library](https://github.com/jie-meng/Util).

2. Run `python3 install_build_tool_here.py`. You would be ask `Please input Util project dir:`, then input Util absolute directory you justed cloned/downloaded above. Something like: `/Users/XXX/projects/Util`.
 
#### Build Qscintilla ####

1. cd QScintilla/Qt4Qt5

2. qmake qscintilla.pro

3. make

4. make install

#### Build Lunar ####

Run `./luaexec remake_lunar.lua`.

If you already built it then made some changes and just want an incremental build, run `./luaexec make_lunar.lua`.

When you add/remove an icon or add/remove a source file which need to regenerate Lunar.pro, you should always clear everything and start from the begginning, run `./luaexec clear_lunar.lua`, and then `./luaexec remake_lunar.lua`.

### Release steps ###

On Linux and Windows 

- Make a dir named Lunar, lets' call it LUNAR_HOME. 
- Put all the files and folders in publish to LUNAR_HOME.
- Put luaexec and Lunar to LUNAR_HOME.

On windows extras (You can find all the dlls in Qt installation directory):

- Put libstdc++-6.dll(mingw), libgcc_s_dw2-1.dll(mingw), libwinpthread-1.dll(mingw), Qt5Core.dll, Qt5Gui.dll,  Qt5PrintSupport.dll, Qt5Widgets.dll, qscintilla2_qt5.dll, icudt54, icuin54.dll, icuuc54.dll to LUNAR_HOME.
- Put qwindows.dll to LUNAR_HOME/platforms

On MacOS

- Run `./luaexec deploy_on_mac.lua` on terminal to make Lunar.dmg

- In the making-dmg step, you'll need to give some information:

    - `Please input libqscintilla2_qt5.13.dylib dir:` You need to input the ibqscintilla2_qt5.13.dylib installed directory, something like `/Users/XXX/programs/Qt5.7.1/5.7/clang_64/lib`
    
    - `Deploy as dmg? (y/n)` 
        - Type `y` for generating dmg. Then you got Lunar.dmg.
        - Type `n` for not generating dmg, then you only got a Lunar.app, you can run `./luaexec replace_current_lunar_on_mac.lua` to replace /Applications/Lunar.app with this one. This is very useful when you want perform a quick-test after some modification and build.

### Install Lunar ###

For Linux users, you should build Lunar following the Release steps.

For MacOS or Windows users, you can download installation-package from the following link:

[Download Link](https://github.com/jie-meng/Lunar/releases)

- MacOS

    - Install Lunar from Lunar.dmg to `/Applications`
    - Add `export PATH=$PATH:/Applications/Lunar.app/Contents/MacOS` to your environment. (Then you can start Lunar from your terminal in current directory by typing command `luna`)
    
- Windows

    - Just unzip lunar_win.zip
    - Add lunar_win directory to your environment PATH.
    
- Linux

    - After Lunar is built and everything in a folder, add the folder directory to environment: `export PATH=$PATH:{LUNAR_HOME}`
    
After installation, you should update plugins at once.

### Update plugins ###

- Open Lunar from command line. (For Linux and Windows, input `Lunar` then press `Enter`. for MacOS input `luna` then press `Enter`)

- Open File Explorer with shortcut key `CTRL (COMMAND on MacOS) + SHIFT + E`, then `CTRL (COMMAND on MacOS) + J` open Context Menu. Select `Tools/lunar/update_lunar_plugins` to update packages (apis / assets / plugins / tools / luaexeclib / extension.lua) to the latest. (Make sure you have svn command line installed). 

- When execute the update, you would be asked `Are you sure to replace extension.lua which may change your current settings? (y/n)`.
    
    - Type `y` and click `input` button (or press `Enter`): you would use new `extension.lua` from the download. Your old `extension.lua` would be kept as `extension_backup.lua`. For a new-install-user, just type `y`.
    
    - Type `n` and click `input` button (or press `Enter`): you would keep your old `extension.lua`. New-downloaded `extension.lua` would be saved as `extension_latest.lua`. If you made some changes to your `extension.lua` and want to keep the changes, select `n` to keep your old `extension.lua`, you can compare it with the newest one `extension_latest.lua` after update.

### Notes ###

- V1.9.3 (2018.01.25)

    - Add 'inspect' to python default lib in pydoc_gen.lua
    - Solve findfiles sometimes crash bug
    - CPP goto-definition plugin enhancement
    - Make shortcut key CTRL+R Replace
    - Fix finddialog sometimes hide by mainwindow bug
    - Merge extension_tools to tools
    - Support set project path to sub dir in FileExplorer

- V1.9.2 (2018.01.18)

    Fix cannot edit and save lunar.cfg in Lunar bug. Support goto Lunar settings.

- V1.9.1 (2018.01.12)

    Support plugins and tools auto update: Use tools/update_lunar_plugins.lua

- V1.9.0 (2018.01.11)

    Update Qscintilla version to 2.10.2; python2/3 plugins make greatly enhancement. (use pydoc_gen.lua in apis/python to generate api first)

- V1.8.8 (2017.11.14)
    
    Support locate current file in file explorer.

- V1.8.7 (2017.11.12)
    
    Support find files.
    
- V1.8.6 (2017.11.08)
	
    Support start from MAC terminal with command 'luna'

- V1.8.3 (2017.08.17)

    Support run recent script with F6.
    
- V1.8.1 (2017.05.21)
    
    Support cocos2dx-js.
    
- V1.7.7 (2017.03.01)

    Update QScintilla2 to 2.10. Support Markdown & JSON.

- V1.7.2 (2016.12.29)
	
    Update util to V1.0.5. All util extend api reformed.
    
- V1.5.8 (2016.06.28)
    
    Support cpp plugins.

- V1.5.3 (2016.05.20)

    Support MacOS.
    
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
