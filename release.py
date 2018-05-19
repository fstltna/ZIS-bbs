#!/usr/bin/env python2

# Parts shamelessly stolen from Tools/scripts/fixnotice.py from Python 2.2
# distribution ;-)

"""Updates occurencess of current revision in lines containing a magic marker.

Processes all files of current directory (non-recursively), except files
ending with `~'.  Backs up changed files with `~' suffix."""

# This line will be updated.  This script must be writable!

OLD_RELEASE = '0.99.5' # @RELEASE@

MAGIC = "@" + "RELEASE" + "@" # avoid magic in bytecode (.pyc)

import os, sys, stat, shutil

def main():
    global NEW_RELEASE, OLD_RELEASE

    args = sys.argv[1:]

    if len(args) not in (1, 2):
        print ("No arguments.\n"
               "Usage: ./fixnotice.py [OLD_RELEASE] NEW_RELEASE\n")
        sys.exit(1)
    else:
        if len(args) == 1:
            NEW_RELEASE = args[0]
        else:
            OLD_RELEASE, NEW_RELEASE = args

    print "Replacing `%s' by `%s'..." % (OLD_RELEASE, NEW_RELEASE)

    for fname in os.listdir('.'):
        if fname[-1] != '~' and stat.S_ISREG(os.stat(fname)[stat.ST_MODE]):
            process(fname)

def process(fname):
    try:
        f = file(fname)
    except IOError, ex:
        print ex
        return
    data = f.read()
    f.close()

    if data.find(MAGIC) < 0:
##      print "No magic in", fname
        return

    # Note that the newlines are stripped.
    lines = data.split('\n')
    
    for line, num in zip(lines, range(len(lines))):
        if line.find(MAGIC) < 0:
            continue
        if line.find(OLD_RELEASE) < 0:
            print "Magic found but `%s' missing!!!"
        else:
            lines[num] = line = line.replace(OLD_RELEASE, NEW_RELEASE)
        print "%s:%d:%s" % (fname, (num + 1), line)

    new = fname + '.new'
    backup = fname + '~'

    f = file(new, "w")
    f.write('\n'.join(lines))
    f.close()

    shutil.copymode(fname, new)

    os.rename(fname, backup)
    os.rename(new, fname)

if __name__ == '__main__':
    main()

