print("Please input libqscintilla2_qt5.13.dylib dir:")
local libdir = io.read()

print("Copy plugins/apis/extension_tools/scripts/config ..")
os.execute("cp -r publish/. Lunar.app/Contents/MacOS")

print("Copy *.dylib to executable path ..")
os.execute(string.format("cp %s/*.dylib Lunar.app/Contents/MacOS", libdir))

print("Copy start lunar from command line sh ..")
os.execute("cp ./luna Lunar.app/Contents/MacOS")

print("install name tool")
os.execute("install_name_tool -change libqscintilla2_qt5.13.dylib @executable_path/libqscintilla2_qt5.13.dylib Lunar.app/Contents/MacOS/Lunar")

print("Deploy as dmg? (y/n)")
local yn = io.read()
if string.lower(yn) == "y" then
    print("macdeployqt Lunar.app -dmg")
    os.execute("macdeployqt Lunar.app -dmg")
end

print("OK")
