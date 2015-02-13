DEBUG_FOLDER = "debug"
RELEASE_FOLDER = "release"
MAKEFILE = "makefile"
MAKEFILE_DEBUG = "makefile.debug"
MAKEFILE_RELEASE = "makefile.release"

function rebuild(pro_file, append_to_pro_file, makefile)
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
    print("Append " .. append_to_pro_file .. " to " .. pro_file)
	file.writeTextFile(pro_file, file.readTextFile(append_to_pro_file), true)	
    print("qmake")
	os.execute("qmake")
	print("qmake successfully!")
	print("mingw32-make")
	os.execute("mingw32-make -f " .. makefile)    
end

-- main
PRO = "Lunar.pro"
ADDTOPRO = "build_config"
MAKEFILE = "makefile.release"
rebuild(PRO, ADDTOPRO, MAKEFILE)
print("rebuild release finished!")

io.read()