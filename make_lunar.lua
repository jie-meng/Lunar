if util.strContains(util.platformInfo(), "unix", false) then
    util.pathRemove(util.currentPath() .. "/Lunar")
    os.execute("make")
    if util.isPathExists(util.currentPath() .. "/Lunar") == true then
        print("make success")
    else
        print("make fail")
    end
    
elseif util.strContains(util.platformInfo(), "windows", false) then
    util.pathRemove(util.currentPath() .. "/release/Lunar.exe")
    os.execute("mingw32-make")

    if util.isPathExists(util.currentPath() .. "/release/Lunar.exe") == true then
        print("make success")
    else
        print("make fail")
    end
else

end
