--[[ To run this script, you should have svn command line installed on your system. For windows, 
    you can install tortoisesvn with its command line]]

print('Are you sure to replace extension.lua which may change your current settings? (y/n)\n')
local is_update_exetension_lua = io.read()

local fpath, _ = util.splitPathname(util.appPath())
util.setCurrentPath(fpath)

print('Make directory download_tmp')
util.pathRemoveAll('download_tmp')
util.mkDir('download_tmp')

print('Download latest plugins and tools from github ...')
util.setCurrentPath(fpath .. '/download_tmp')
local ret = os.execute('svn checkout https://github.com/jie-meng/Lunar/trunk/publish')
if not ret then
    print('\nError: Fetch data failed.')
    return
end
util.pathRemoveAll('publish/.svn')

print('Download latest luaexeclib from github ...')
ret = os.execute('svn checkout https://github.com/jie-meng/Util/trunk/luaexeclib')
if not ret then
    print('\nError: Fetch data failed.')
    return
end
util.pathRemoveAll('luaexeclib/.svn')

util.setCurrentPath(fpath)
print('Update local plugins and tools ...')
local files = util.findFilesInDirRecursively(fpath .. '/download_tmp/publish')
for _, v in ipairs(files) do
    if v == fpath .. '/download_tmp/publish/extension.lua' then
        if util.strStartWith(is_update_exetension_lua, 'y', false) then
            util.pathRemove(fpath .. '/extension_backup.lua')
            util.pathRename(fpath .. '/extension.lua', fpath .. '/extension_backup.lua')
            util.fileCopyFullPath(v, fpath .. '/extension.lua')
            if util.readTextFile(fpath .. '/extension.lua') == util.readTextFile(fpath .. '/extension_backup.lua') then
                util.pathRemove(fpath .. '/extension_backup.lua')
            end
        else
            util.fileCopyFullPath(v, fpath .. '/extension_latest.lua')
            if util.readTextFile(fpath .. '/extension.lua') == util.readTextFile(fpath .. '/extension_latest.lua') then
                util.pathRemove(fpath .. '/extension_latest.lua')
            end
        end
    else
        util.fileCopyFullPath(v, util.strReplace(v, 'download_tmp/publish/', ''))
    end
end

print('Update local luaexeclib ...')
util.copyTree(fpath .. '/download_tmp/luaexeclib', fpath .. '/luaexeclib')

print('Download latest install_luaexeclib.lua from github ...')
ret = os.execute('svn export --force https://github.com/jie-meng/Util/trunk/install_luaexeclib.lua')
if not ret then
    print('\nError: Fetch data failed.')
    return
end

print('Install luaexeclib ...')
if util.strContains(util.platformInfo(), 'win', false) then
    os.execute('luaexec.exe install_luaexeclib.lua')
else
    os.execute('./luaexec install_luaexeclib.lua')
end

print('Clean up ...')
util.pathRemoveAll('download_tmp')

print('\nDone!')
