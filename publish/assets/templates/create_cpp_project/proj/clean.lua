util.pathRemove('CppApplicationName')
util.pathRemove('CppApplicationName.exe')
util.pathRemove('Makefile')
util.pathRemove('CMakeCache.txt')
util.pathRemove('cmake_install.cmake')
util.pathRemoveAll('CMakeFiles')
util.pathRemoveAll('Debug')
util.pathRemoveAll('Release')
util.pathRemoveAll('debug')
util.pathRemoveAll('release')

print('Clean ok.')

