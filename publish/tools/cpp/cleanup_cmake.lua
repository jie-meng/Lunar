function clearDir(dir)
    util.pathRemoveAll(dir .. '/CMakeFiles')
    util.pathRemove(dir .. '/Makefile')
    util.pathRemove(dir .. '/CMakeCache.txt')
    util.pathRemove(dir .. '/cmake_install.cmake')

    print('cleanup ' .. dir .. ' ...')
end

function recursiveCleanupCMakeCache(path)
    local tb = util.findPathInDir(path)
    local filtered_tb = {}
    for _, v in ipairs(tb) do
        if not util.strStartWith(util.fileBaseName(v), '.') and util.fileBaseName(v) ~= 'CMakeFiles' then
            table.insert(filtered_tb, v)
        end
    end

    for _, v in ipairs(filtered_tb) do
       recursiveCleanupCMakeCache(v)
    end

    clearDir(path)
end

recursiveCleanupCMakeCache(util.currentPath())

print('\ndone')