if strContains(platformInfo(), "unix", false) then
    os.execute("cocos compile -p linux")
elseif strContains(platformInfo(), "windows", false) then
    os.execute("cocos compile -p win32")
else
    print("cannot compile")
end

print("compile finish")