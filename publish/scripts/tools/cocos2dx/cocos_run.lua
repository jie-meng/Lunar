function cocosRun(name)

    if strContains(platformInfo(), "unix", false) then
        os.execute(file.currentPath() .. "/runtime/linux/" .. name)
    elseif strContains(platformInfo(), "windows", false) then
        os.execute(file.currentPath() .. "/runtime/win32/" .. name .. ".exe")
    else
        print("cannot run")
    end

    print("run finish")
end

-- main
cocosRun("LCocos")