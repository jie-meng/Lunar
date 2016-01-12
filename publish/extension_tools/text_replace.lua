print(string.format("Replace text in path <%s>", file.currentPath()))

print()

print("Input target file ext (Empty means all files):")
local ext = strTrim(io.read())
print("Ext: " .. ext)

print("Input find text:")
local find_text = strTrim(io.read())
if find_text == "" then
    print("Error: Find text cannot be empty")
    os.exit(0)
end
print("Find text: " .. find_text)

print("Input replace text:")
local replace_text = strTrim(io.read())
print("Replace text: " .. replace_text)
print()

local tb = file.findFilesInDirRecursively(file.currentPath(), ext)
for _, v in ipairs(tb) do
    local text = file.readTextFile(v)
    if strContains(text, find_text) then
        local replace = strRelaceAll(text, find_text, replace_text)
        if file.writeTextFile(v, replace) then
            print(string.format("Replace in file %s ok.", v))
        end
    end
end

print()

print(string.format([[Replace all "%s" to "%s" in path <%s> ok.]], find_text, replace_text, file.currentPath()))
