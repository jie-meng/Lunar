util.pathRemoveAll("debug")
print("remove dir debug")

util.pathRemoveAll("release")
print("remove dir release")

util.pathRemoveAll("Lunar.app")
print("remove dir Lunar.app")

util.pathRemoveAll("deploylibs")
print("remove dir deploylibs")

util.pathRemoveAll("luaexeclib")
print("remove dir luaexeclib")

if util.pathRemove("Lunar.dmg") then
    print("remove Lunar.dmg")
end

if util.pathRemove("Makefile") then
    print("remove Makefile")
end

if util.pathRemove("Makeutil.Debug") then
    print("remove Makeutil.Debug")
end

if util.pathRemove("Makeutil.Release") then
    print("remove Makeutil.Release")
end

if util.pathRemove("object_script.Lunar.Debug") then
    print("remove object_script.Lunar.Debug")
end

if util.pathRemove("object_script.Lunar.Release") then
    print("remove object_script.Lunar.Release")
end

if util.pathRemove("qrc_Lunar.cpp") then
    print("remove qrc_Lunar.cpp")
end

if util.pathRemove("Lunar.pro") then
    print("remove Lunar.pro")
end

if util.pathRemove(".qmake.stash") then
    print("remove .qmake.stash")
end

if util.pathRemove("Lunar") then
    print("remove Lunar")
end

if util.pathRemove("luaexec") then
    print("remove luaexec")
end

if util.pathRemove("install_luaexeclib.lua") then
    print("remove install_luaexeclib.lua")
end

t = util.findFilesInDir(".")

for k, v in pairs(t) do

    local path, fileName = util.splitPathname(v)
    if util.fileExtension(fileName) == "o" then
        print("remove " .. v)
        util.pathRemove(v)
    end
    local i, j = string.find(fileName, "moc")
    if i == 1 and j == 3 then
        print("remove " .. v)
        util.pathRemove(v)
    end
end

print("clear successfully!")
