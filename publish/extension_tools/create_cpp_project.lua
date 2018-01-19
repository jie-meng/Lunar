local app_dir, _ = util.splitPathname(util.appPath())
local model_path = app_dir .. '/assets/templates/create_cpp_project/proj'

local dst_dir = util.currentPath()

print('Please input app name:')
local app_name = io.read()
local dst_path = dst_dir .. '/' .. app_name
if util.isPathDir(dst_path) then
    print(string.format('Error: "%s" already exists.', dst_path))
    os.exit(0)
end

if not util.mkDir(dst_path) then
    print(string.format('Error: create project "%s" failed.', dst_path))
    os.exit(0)
end

local mfiles = util.findFilesInDir(model_path)
for _, v in ipairs(mfiles) do
    local d, f = util.splitPathname(v)
    local content = util.readTextFile(v)
    content = util.strReplaceAll(content, 'CppApplicationName', app_name)
    local dst_file = dst_path .. '/' .. f
    if util.writeTextFile(dst_file, content) then
        print(string.format('Generate file "%s" ok.', dst_file))
    end
end

if util.mkDir(dst_path .. '/src') then
    if util.fileCopy(model_path .. '/src/main.cpp', dst_path .. '/src/main.cpp') then
        print(string.format('Generate file "%s" ok.', dst_path .. '/src/main.cpp'))
    end
end

print('')
print(string.format('Create cpp project "%s" ok.', dst_path))

