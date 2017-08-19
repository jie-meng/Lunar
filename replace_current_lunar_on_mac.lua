print('copy cfg')
os.execute('cp /Applications/Lunar.app/Contents/MacOS/cfg Lunar.app/Contents/MacOS/')
print('rm /Applications/Lunar.app')
os.execute('rm -rf /Applications/Lunar.app')
print('mv Lunar.app to /Applications')
os.execute('mv Lunar.app /Applications')

print('done')