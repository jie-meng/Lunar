--compile
--os.execute("cocos compile -p win32")
--os.execute("cocos compile -p linux")

--run
os.execute(file.currentPath() .. "/runtime/win32/LCocos.exe")
--os.execute(file.currentPath() .. "/runtime/linux/LCocos")

print("ok")