local files = util.findFilesInDirRecursively(util.currentPath(), 'sh')
for _, v in ipairs(files) do
    print('chmod a+x ' .. v)
    os.execute('chmod a+x ' .. v)
end

print('\ndone!')