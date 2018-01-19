import os
import tarfile

def getTarFiles(path):
    gz_files = []
    for root, dirs, files in os.walk(path):
        for f in files:
            if f.endswith('tar.gz'):
                gz_files.append(root + os.sep + f)
                return gz_files

def extractTarMFile(file, output_path):
    tar = tarfile.open(file)
    names = tar.getnames()
    for name in names:
        if name.endswith('.m'):
            tar.extract(name, output_path)
    
    tar.close()

# extract octave mfiles
files = getTarFiles(os.getcwd())
for f in files:
    extractTarMFile(f, './mfiles')

print('\ndone!')
