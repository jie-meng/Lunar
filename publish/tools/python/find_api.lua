local app_path, _ = util.splitPathname(util.appPath())

local folder = 'pydoc_gen'
print('Is python3 (y/n)?')
local pyversion = io.read()
if util.strStartWith(pyversion, 'y', false) then
    folder = folder .. '3'
end

print('Input api you want are looking for:')
local api = io.read()

if string.len(util.strTrim(api)) == 0 then
    print('You are looking for nothing.')
    os.exit(-1)
end

print(string.format('Input package you want to look for "%s" in:', api))
local package = io.read()

if string.len(util.strTrim(package)) == 0 then
    print(string.format('You are looking for "%s" in all packages ...', api))
    package = nil
else
    print(string.format('You are looking for "%s" in "%s" package ...', api, package))
end

print()

local api_path = app_path .. '/apis/python/' .. folder
local files = util.findFilesInDir(api_path)

local result = {}

for _, filename in ipairs(files) do
    local _, name = util.splitPathname(filename)    
    if not package or util.strStartWith(name, package) then
        local f = io.open(filename, "r")
        if f then
            local line = f:read("*line")
            while line do
                repeat
                    if string.len(util.strTrim(line)) == 0 then
                        break
                    end

                    if util.strContains(line, api) then
                        table.insert(result, { module = name, api = line })
                    end
                until true
                line = f:read("*line")
            end
            io.close()
        end
    end
end

for _, v in ipairs(result) do
    print(string.format('[%s] %s', v.module, v.api))
end

print('\n\ndone!')
