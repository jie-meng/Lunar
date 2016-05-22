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
	print("qmake -project")
	os.execute("qmake -project")
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
