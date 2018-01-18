print('copy lunar.cfg')
os.execute('cp /Applications/Lunar.app/Contents/MacOS/lunar.cfg Lunar.app/Contents/MacOS/')

if util.isPathFile('/Applications/Lunar.app/Contents/MacOS/apis/python/pydoclibs') then
    print('copy pydoclibs')
    os.execute('cp /Applications/Lunar.app/Contents/MacOS/apis/python/pydoclibs Lunar.app/Contents/MacOS/apis/python/')
end

if util.isPathFile('/Applications/Lunar.app/Contents/MacOS/apis/python/pydoclibs3') then
    print('copy pydoclibs3')
    os.execute('cp /Applications/Lunar.app/Contents/MacOS/apis/python/pydoclibs3 Lunar.app/Contents/MacOS/apis/python/')
end

print('rm /Applications/Lunar.app')
os.execute('rm -rf /Applications/Lunar.app')

print('mv Lunar.app to /Applications')
os.execute('mv Lunar.app /Applications')

print('done')
