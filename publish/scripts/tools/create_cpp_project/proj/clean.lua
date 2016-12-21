file.pathRemove('CppApplicationName')
file.pathRemove('CppApplicationName.exe')
file.pathRemove('Makefile')
file.pathRemove('CMakeCache.txt')
file.pathRemove('cmake_install.cmake')
file.pathRemoveAll('CMakeFiles')
file.pathRemoveAll('Debug')
file.pathRemoveAll('Release')
file.pathRemoveAll('debug')
file.pathRemoveAll('release')

print('Clean ok.')

