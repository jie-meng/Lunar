import os
import shutil

if not os.path.isfile("./luaexec") or not os.path.isdir("./luaexeclib") or not os.path.isfile("./install_luaexeclib.lua"):
    shutil.rmtree("./luaexec", True)
    shutil.rmtree("./install_luaexeclib.lua", True)
    shutil.rmtree("./luaexeclib", True)
    
    prev_dir = os.getcwd()
    util_dir = input("Please input Util project dir:")
    os.chdir(util_dir)
    print("Make luaexec ...")
    os.system("python make.py")
    if not os.path.isfile("./luaexec"):
        os.exit(-1)
    os.chdir(prev_dir)
    
    print("Copy luaexec & luaexeclib here ...")
    shutil.copyfile(util_dir + "/luaexec", os.getcwd() + "/luaexec")
    shutil.copyfile(util_dir + "/install_luaexeclib.lua", os.getcwd() + "/install_luaexeclib.lua")
    shutil.copytree(util_dir + "/luaexeclib", os.getcwd() + "/luaexeclib")
    
    print("Give luaexec execute priority ...")
    os.system("chmod a+x ./luaexec")

print('done')
