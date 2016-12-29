if util.strContains(platformInfo(), "unix", false) then
    os.execute("cocos run -p linux")
elseif util.strContains(platformInfo(), "windows", false) then
    os.execute("cocos run -p win32")
else
    print("cannot compile")
end

print("compile finish")