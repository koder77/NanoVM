import subprocess
import sys
import re
import os.path
import shutil

def main():
    statify(sys.argv[1], sys.argv[2])

def statify(binaryPath, outputPath):
    copiedLibrariesNumber = 0

    for library in detectUsedLibraries(binaryPath):
        try:
            shutil.copyfile(library.path, os.path.join(outputPath, library.name))

            copiedLibrariesNumber += 1
        except Exception as e:
            print 'Failed to copy library %s: %s' % (library.name, e)

    print 'Copied %s libraries' % (copiedLibrariesNumber,)

class Library(object):

    def __init__(self, name, path):
        self.name = name
        self.path = path

    def __str__(self):
        return '[%s]' % (self.name,)

class LddLibraryParser(object):

    def __init__(self):
        #         libxcb-render.so.0 => /usr/lib/i386-linux-gnu/libxcb-render.so.0 (0xb462a000)
        self.namePathPattern = re.compile('[\s]*([^ /]+) => ([^ ]+).*')

        #         /lib/ld-linux.so.2 (0xb7785000)
        self.pathPattern = re.compile('[\s]*([^ ]+) [(]0x[\w]+[)]')

    def parse(self, line):
        library = self.parseNamePathLine(line)
        if(not library is None):
            return library

        library = self.parsePathLine(line)
        if(not library is None):
            return library
        
        raise Exception('Illegal line format: %s' % (line,))

    def parseNamePathLine(self, line):
        m = self.namePathPattern.match(line)

        if(not m):
            return None

        return Library(m.group(1), m.group(2))

    def parsePathLine(self, line):
        m = self.pathPattern.match(line)

        if(not m):
            return None

        path = m.group(1)

        return Library(os.path.basename(path), path)

def detectUsedLibraries(binaryPath):
    parse = LddLibraryParser().parse
    return [parse(line) for line in subprocess.check_output(['ldd', binaryPath]).split('\n') if not len(line.strip()) == 0]

if __name__ == '__main__':
    main()
