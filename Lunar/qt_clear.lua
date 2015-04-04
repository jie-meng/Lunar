file.pathRemoveAll("debug")
print("remove dir debug")

file.pathRemoveAll("release")
print("remove dir release")

if file.pathRemove("Makefile") then
    print("remove Makefile")
end

if file.pathRemove("qrc_Lunar.cpp") then
    print("remove qrc_Lunar.cpp")
end

if file.pathRemove("Lunar.pro") then
    print("remove Lunar.pro")
end

if file.pathRemove("Lunar") then
    print("remove Lunar")
end

t = file.findFilesInDir(".")

for k, v in pairs(t) do

    local path, fileName = file.splitPathname(v)
    if file.fileExtension(fileName) == "o" then
        print("remove " .. v)
        file.pathRemove(v)
    end
    local i, j = string.find(fileName, "moc")
    if i == 1 and j == 3 then
        print("remove " .. v)
        file.pathRemove(v)
    end
end

print("clear successfully!")
