print('What language do you want to override? (lua / python / javascript / ruby / ...)\n')
local type = io.read()
local file = util.currentPath() .. '/luna_ext_' .. type .. '.json'

if util.isPathFile(file) then
    print(file .. ' already there!')
    os.exit(0)
end

local content = string.format([[{
    "auto_complete_type": 1,
    "api": "apis/%s,./luna_override", 
    "executor": "%s",
    "plugin_goto": "./luna_override/goto_%s.lua",
    "plugin_parse_api": "./luna_override/parse_%s_supplement_api.lua",
    "templates": "templates/%s"
}]], type, type, type, type, type)

if util.writeTextFile(file, content) then
    print('generate ' .. file .. ' done!')
else
    print('generate ' .. file .. ' failed!')
end
