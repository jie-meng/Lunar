-- This script can only run on unix shell

local fpath, _ = util.splitPathname(util.appPath())

util.setCurrentPath(fpath)

print('Make directory download_tmp')
util.pathRemoveAll('download_tmp')
util.mkDir('download_tmp')

print('Download latest plugins and tools from github ...')
util.setCurrentPath(fpath .. '/download_tmp')
os.execute('svn checkout https://github.com/jie-meng/Lunar/trunk/publish')
os.execute('rm -rf publish/.svn')

print('Update local plugins and tools ...')
util.setCurrentPath(fpath)
os.execute('cp -r download_tmp/publish/apis/. apis')
os.execute('cp -r download_tmp/publish/config/. config')
os.execute('cp -r download_tmp/publish/extension_tools/. extension_tools')
os.execute('cp -r download_tmp/publish/plugins/. plugins')
os.execute('cp -r download_tmp/publish/tools/. tools')

print('Are you sure to replace extension.lua which may change your current settings? (y/n)\n')
local answer = io.read()
if util.strStartWith(answer, 'y', false) then
    os.execute('cp download_tmp/publish/extension.lua .')
else
    os.execute('mv download_tmp/publish/extension.lua extension_latest.lua')
end

print('Clean up ...')
util.pathRemoveAll('download_tmp')

print('\nDone!')