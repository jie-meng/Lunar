local app_path, _ = util.splitPathname(util.appPath())
local proj_dir = util.currentPath()

print('copy files to ' .. proj_dir .. ' ...')

os.execute(string.format('cp -r %s/assets/templates/cocos2dx/cocos2dx-js-webpack/proj/. %s', app_path, proj_dir))
os.execute('rm -rf ' .. proj_dir .. '/src')
os.execute('mkdir ' .. proj_dir .. '/src')

print('\ndone!')
