--[[ To run this script, you should have svn command line installed on your system. For windows, 
    you can install tortoisesvn with its command line]]

function forceCopyFile(src, dst)
    local path, _ = util.splitPathname(dst)
    if not (util.strEndWith(path, '.svn') or util.strEndWith(path, '.git')) then
        if not util.isPathDir(path) then
            util.mkFullDir(path)
        end
        util.fileCopy(src, dst, false)    
    end
end

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
            forceCopyFile(v, fpath .. '/extension.lua')
        else
            forceCopyFile(v, fpath .. '/extension_latest.lua')
        end
    else
        forceCopyFile(v, util.strReplace(v, 'download_tmp/publish/', ''))
    end
end

print('Update local luaexeclib ...')
if not util.isPathDir('luaexeclib') then
    util.mkDir('luaexeclib')
end
local files = util.findFilesInDirRecursively(fpath .. '/download_tmp/luaexeclib')
for _, v in ipairs(files) do
    forceCopyFile(v, util.strReplace(v, 'download_tmp/', ''))
end

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
