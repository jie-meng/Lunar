print("Please input libqscintilla2.12.dylib dir:")
local libdir = io.read()

print("install name tool")
os.execute(string.format("install_name_tool -change libqscintilla2.12.dylib %s/libqscintilla2.12.dylib Lunar.app/Contents/MacOS/Lunar", libdir))

print("macdeployqt Lunar.app")
os.execute("macdeployqt Lunar.app")

print("Copy plugins/apis/extension_tools/scripts/config ..")
os.execute("cp -r publish/. Lunar.app/Contents/MacOS")

print("OK")
