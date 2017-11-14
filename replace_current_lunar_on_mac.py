import os

print('copy lunar.cfg')
os.system('cp /Applications/Lunar.app/Contents/MacOS/lunar.cfg Lunar.app/Contents/MacOS/')
print('rm /Applications/Lunar.app')
os.system('rm -rf /Applications/Lunar.app')
print('mv Lunar.app to /Applications')
os.system('mv Lunar.app /Applications')

print('done')
