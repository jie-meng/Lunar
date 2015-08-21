if strContains(platformInfo(), "unix", false) then
    file.pathRemove(file.currentPath() .. "/Lunar")
    os.execute("make")
    if file.isPathExists(file.currentPath() .. "/Lunar") == true then
        print("make success")
    else
        print("make fail")
    end
    
elseif strContains(platformInfo(), "windows", false) then
    file.pathRemove(file.currentPath() .. "/release/Lunar.exe")
    os.execute("mingw32-make")

    if file.isPathExists(file.currentPath() .. "/release/Lunar.exe") == true then
        print("make success")
    else
        print("make fail")
    end
else

end
