file.pathRemove(file.currentPath() .. "/release/Lunar.exe")
os.execute("mingw32-make")

if file.isPathExists(file.currentPath() .. "/release/Lunar.exe") == true then
    print("make success")
else
    print("make fail")
end
