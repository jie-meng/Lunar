if util.strContains(util.platformInfo(), "unix", false) then
    if util.strContains(util.platformInfo(), "macos", false) then
        --mac
        util.pathRemoveAll(util.currentPath() .. "/Lunar.app")
        os.execute("make")
        if util.isPathDir(util.currentPath() .. "/Lunar.app") == true then
            print("make success")
        else
            print("make fail")
        end
    else
        --unix
        util.pathRemove(util.currentPath() .. "/Lunar")
        os.execute("make")
        if util.isPathExists(util.currentPath() .. "/Lunar") == true then
            print("make success")
        else
            print("make fail")
        end
    end
elseif util.strContains(util.platformInfo(), "windows", false) then
    --windows
    util.pathRemove(util.currentPath() .. "/release/Lunar.exe")
    os.execute("mingw32-make")

    if util.isPathExists(util.currentPath() .. "/release/Lunar.exe") == true then
        print("make success")
    else
        print("make fail")
    end
end
