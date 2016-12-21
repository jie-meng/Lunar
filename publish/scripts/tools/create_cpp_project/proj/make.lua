local Tools = require("tools")

if Tools.isPlatformWindows() then
    os.execute('cmake -G "MinGW Makefiles"')
    os.execute('mingw32-make')
else
    os.execute('cmake .')
    os.execute('make')
end

print('Make ok.')
