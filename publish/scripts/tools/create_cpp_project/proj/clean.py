import os
import shutil

def removeFile(f):
    if os.path.isfile(f):
        os.remove(f)

removeFile("CppApplicationName")
removeFile("CppApplicationName.exe")
removeFile("Makefile")
removeFile("CMakeCache.txt")
removeFile("cmake_install.cmake")
shutil.rmtree("CMakeFiles", True)
shutil.rmtree("Debug", True)
shutil.rmtree("Release", True)
shutil.rmtree("debug", True)
shutil.rmtree("release", True)

print("Clean ok.")

