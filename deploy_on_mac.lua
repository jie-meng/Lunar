print("Copy plugins/apis/extension_tools/scripts/config ..")
os.execute("cp -r publish/. Lunar.app/Contents/MacOS")

if not util.isPathDir("deploytmp") then
    util.mkDir("deploytmp")
end

if #util.findFilesInDir("deploytmp", "dylib") == 0 then
    print("Please input libqscintilla2_qt5.13.dylib dir:")
    local libdir = io.read()
    
    print("Copy *.dylib to deploytmp ..")
    os.execute(string.format("cp %s/*.dylib deploytmp", libdir))    
end

print("Copy *.dylib to executable path ..")
os.execute("cp deploytmp/*.dylib Lunar.app/Contents/MacOS")

if not util.isPathFile("deploytmp/luaexec") then
    print("Please input Util project dir:")
    local prev_dir = util.currentPath()
    local util_dir = io.read()
    util.setCurrentPath(util_dir)
    print("Make luaexec ...")
    os.execute("python make.py")
    if not util.isPathFile("luaexec") then
        os.exit(-1)
    end
    util.setCurrentPath(prev_dir)
    
    print("Copy luaexec & luaexeclib to deploytmp ...")
    os.execute(string.format("cp %s/luaexec deploytmp", util_dir))
    os.execute(string.format("cp -r %s/luaexeclib deploytmp", util_dir))
    os.execute(string.format("cp %s/install_luaexeclib.lua deploytmp", util_dir))
end

print("Copy luaexec & luaexeclib to executable path ...")
os.execute("cp deploytmp/luaexec Lunar.app/Contents/MacOS")
os.execute("cp -r deploytmp/luaexeclib Lunar.app/Contents/MacOS")
os.execute("cp deploytmp/install_luaexeclib.lua Lunar.app/Contents/MacOS")

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
