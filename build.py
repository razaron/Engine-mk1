#!/usr/bin/python

import sys
import os
import shutil
import errno

options = " -DCMAKE_BUILD_TYPE=Debug";

# create options string
if len(sys.argv) > 1:
    options = " -DCMAKE_BUILD_TYPE=" + sys.argv[1]
if len(sys.argv) > 2:
    options += " -DENVIRONMENT=" + sys.argv[2]

# cd to correct folder
if (len(sys.argv) == 1) or (sys.argv[1].find("Debug") != -1):
    try:
        os.mkdir("Debug")
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise  # raises the error again
        else:
            shutil.rmtree("Debug")
            os.mkdir("Debug")
            pass

    os.chdir("Debug")
elif sys.argv[1].find("Release") != -1:
    try:
        os.mkdir("Release")
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise  # raises the error again
        else:
            shutil.rmtree("Release")
            os.mkdir("Release")
            pass

    os.chdir("Release")

os.system("cmake"+options+" ..")
os.system("make")

exit(0)
