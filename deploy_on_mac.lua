print("Copy plugins/apis/extension_tools/scripts/config ..")
os.execute("cp -r publish/. Lunar.app/Contents/MacOS")

if not util.isPathDir("deploylibs") then
    util.mkDir("deploylibs")
end

if #util.findFilesInDir("deploylibs", "dylib") == 0 then
    print("Please input libqscintilla2_qt5.13.dylib dir:")
    local libdir = io.read()
    
    print("Copy *.dylib to deploylibs ..")
    os.execute(string.format("cp %s/*.dylib deploylibs", libdir))    
end

print("Copy *.dylib to executable path ..")
os.execute("cp deploylibs/*.dylib Lunar.app/Contents/MacOS")

if not util.isPathFile("./luaexec") or not util.isPathDir("./luaexeclib") or not util.isPathFile("./install_luaexeclib.lua") then
    print("Cannot find luaexec locally, please run 'python3 install_build_tool_here.py' first.")
    os.exit(-1)
end

print("Copy luaexec & luaexeclib to executable path ...")
os.execute("cp luaexec Lunar.app/Contents/MacOS")
os.execute("cp install_luaexeclib.lua Lunar.app/Contents/MacOS")
os.execute("cp -r luaexeclib Lunar.app/Contents/MacOS")

print("Install luaexeclib ...")
local prev_dir = util.currentPath()
util.setCurrentPath(prev_dir .. "/Lunar.app/Contents/MacOS")
os.execute("./luaexec install_luaexeclib.lua")
util.setCurrentPath(prev_dir)

print("Copy start lunar from command line sh ..")
os.execute("cp ./luna Lunar.app/Contents/MacOS")

print("install name tool")
os.execute("install_name_tool -change libqscintilla2_qt5.13.dylib @executable_path/libqscintilla2_qt5.13.dylib Lunar.app/Contents/MacOS/Lunar")

print("Deploy as dmg? (y/n)")
local yn = io.read()
if string.lower(yn) == "y" then
    print("macdeployqt Lunar.app -dmg")
    os.execute("macdeployqt Lunar.app -dmg")
end

print("OK")
