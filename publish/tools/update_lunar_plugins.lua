--[[ To run this script, you should have svn command line installed on your system. For windows, 
    you can install tortoisesvn with its command line]]

local fpath, _ = util.splitPathname(util.appPath())

util.setCurrentPath(fpath)

print('Make directory download_tmp')
util.pathRemoveAll('download_tmp')
util.mkDir('download_tmp')

print('Download latest plugins and tools from github ...')
util.setCurrentPath(fpath .. '/download_tmp')
os.execute('svn checkout https://github.com/jie-meng/Lunar/trunk/publish')
util.pathRemoveAll('download_tmp/publish/.svn')

print('Download latest luaexeclib from github ...')
os.execute('svn checkout https://github.com/jie-meng/Util/trunk/luaexeclib')
util.pathRemoveAll('download_tmp/luaexeclib/.svn')

util.setCurrentPath(fpath)
print('Update local plugins and tools ...')
local files = util.findFilesInDirRecursively(fpath .. '/download_tmp/publish')
for _, v in ipairs(files) do
    if v == fpath .. '/download_tmp/publish/extension.lua' then
        print('Are you sure to replace extension.lua which may change your current settings? (y/n)\n')
        local answer = io.read()
        if util.strStartWith(answer, 'y', false) then
            util.pathRemove(fpath .. '/extension_backup.lua')
            util.pathRename(fpath .. '/extension.lua', fpath .. '/extension_backup.lua')
            util.fileCopy(v, fpath .. '/extension.lua', false)
        else
            util.fileCopy(v, fpath .. '/extension_latest.lua', false)
        end
    else
        util.fileCopy(v, util.strReplace(v, 'download_tmp/publish/', ''), false)
    end
end

print('Update local luaexeclib ...')
local files = util.findFilesInDirRecursively(fpath .. '/download_tmp/luaexeclib')
for _, v in ipairs(files) do
    util.fileCopy(v, util.strReplace(v, 'download_tmp/luaexeclib/', ''), false)
end

print('Download latest install_luaexeclib.lua from github ...')
os.execute('svn export --force https://github.com/jie-meng/Util/trunk/install_luaexeclib.lua')

print('Install luaexeclib ...')
if util.strContains(util.platformInfo(), 'win', false) then
    os.execute('luaexec.exe install_luaexeclib.lua')
else
    os.execute('./luaexec install_luaexeclib.lua')
end

print('Clean up ...')
util.pathRemoveAll('download_tmp')

print('\nDone!')
