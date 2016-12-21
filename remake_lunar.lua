DEBUG_FOLDER = "debug"
RELEASE_FOLDER = "release"
MAKEFILE = "Makefile"
MAKEFILE_DEBUG = "Makefile.debug"
MAKEFILE_RELEASE = "Makefile.release"

function rebuild(pro_file, append_to_pro_file, makefile, make)
    print("clear...")
	file.pathRemoveAll(DEBUG_FOLDER)
	file.pathRemoveAll(RELEASE_FOLDER)
	file.pathRemove(MAKEFILE)
	file.pathRemove(MAKEFILE_DEBUG)
	file.pathRemove(MAKEFILE_RELEASE)
	file.pathRemove(pro_file)
	print("clear debug,release,makefile successfully!")
    
    print("process cpp/cxx/c/hpp/hxx/h files in publish directory.")
    local publish_files = file.findFilesInDirRecursively("./publish")
    for _, v in ipairs(publish_files) do
        if strEndWith(v, ".cpp", false) or 
            strEndWith(v, ".cxx", false) or 
            strEndWith(v, ".hpp", false) or 
            strEndWith(v, ".hxx", false) or 
            strEndWith(v, ".c", false) or 
            strEndWith(v, ".h", false) then
            file.pathRename(v, v .. ".tmpback")
        end
    end
    
	print("qmake -project")
	os.execute("qmake -project")
    
    print("restore cpp/cxx/c/hpp/hxx/h files in publish directory.")
    local publish_files = file.findFilesInDirRecursively("./publish")
    for _, v in ipairs(publish_files) do
        if strEndWith(v, ".tmpback", false) then
            file.pathRename(v, strLeft(v, string.len(v) - 8))
        end
    end
    
    print("append " .. append_to_pro_file .. " to " .. pro_file)
	file.writeTextFile(pro_file, file.readTextFile(append_to_pro_file), true)
    print("qmake")
	os.execute("qmake")
	print("qmake successfully!")
	print(make)
	os.execute(make .. " -f " .. makefile)   
end

-- main
PRO = "Lunar.pro"

if strContains(platformInfo(), "unix", false) then
    if strContains(platformInfo(), "macos", false) then
        ADDTOPRO = "build_config_macos"
    else
        ADDTOPRO = "build_config_unix"
    end
    MAKE = "make"
elseif strContains(platformInfo(), "windows", false) then 
    ADDTOPRO = "build_config_windows"
    MAKE = "mingw32-make"
else
    print("rebuild release fail!")
    exit(0)
end

rebuild(PRO, ADDTOPRO, MAKEFILE, MAKE)
print("rebuild release finished!")
