print('Please input project directory:')
local proj_dir = io.read()
if not util.isPathDir(proj_dir) then
    print(string.format('Error: Invalid directory "%s".', proj_dir))
    os.exit(0)
end

print('copy files to ' .. proj_dir .. ' ...')
os.execute('cp -r proj/. ' .. proj_dir)
print('done')