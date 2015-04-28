if strContains(platformInfo(), "unix", false) then
    os.execute("cocos run -p linux")
elseif strContains(platformInfo(), "windows", false) then
    os.execute("cocos run -p win32")
else
    print("cannot compile")
end

print("compile finish")