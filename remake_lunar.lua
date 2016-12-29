DEBUG_FOLDER = "debug"
RELEASE_FOLDER = "release"
MAKEFILE = "Makefile"
MAKEFILE_DEBUG = "Makeutil.debug"
MAKEFILE_RELEASE = "Makeutil.release"

function rebuild(pro_file, append_to_pro_file, makefile, make)
    print("clear...")
	util.pathRemoveAll(DEBUG_FOLDER)
	util.pathRemoveAll(RELEASE_FOLDER)
	util.pathRemove(MAKEFILE)
	util.pathRemove(MAKEFILE_DEBUG)
	util.pathRemove(MAKEFILE_RELEASE)
	util.pathRemove(pro_file)
	print("clear debug,release,makefile successfully!")
    
    print("process cpp/cxx/c/hpp/hxx/h files in publish directory.")
    local publish_files = util.findFilesInDirRecursively("./publish")
    for _, v in ipairs(publish_files) do
        if util.strEndWith(v, ".cpp", false) or 
            util.strEndWith(v, ".cxx", false) or 
            util.strEndWith(v, ".hpp", false) or 
            util.strEndWith(v, ".hxx", false) or 
            util.strEndWith(v, ".c", false) or 
            util.strEndWith(v, ".h", false) then
            util.pathRename(v, v .. ".tmpback")
        end
    end
    
	print("qmake -project")
	os.execute("qmake -project")
    
    print("restore cpp/cxx/c/hpp/hxx/h files in publish directory.")
    local publish_files = util.findFilesInDirRecursively("./publish")
    for _, v in ipairs(publish_files) do
        if util.strEndWith(v, ".tmpback", false) then
            util.pathRename(v, util.strLeft(v, string.len(v) - 8))
        end
    end
    
    print("append " .. append_to_pro_file .. " to " .. pro_file)
	util.writeTextFile(pro_file, util.readTextFile(append_to_pro_file), true)
    print("qmake")
	os.execute("qmake")
	print("qmake successfully!")
	print(make)
	os.execute(make .. " -f " .. makefile)   
end

-- main
PRO = "Lunar.pro"

if util.strContains(util.platformInfo(), "unix", false) then
    if util.strContains(util.platformInfo(), "macos", false) then
        ADDTOPRO = "build_config_macos"
    else
        ADDTOPRO = "build_config_unix"
    end
    MAKE = "make"
elseif util.strContains(util.platformInfo(), "windows", false) then 
    ADDTOPRO = "build_config_windows"
    MAKE = "mingw32-make"
else
    print("rebuild release fail!")
    exit(0)
end

rebuild(PRO, ADDTOPRO, MAKEFILE, MAKE)
print("rebuild release finished!")
