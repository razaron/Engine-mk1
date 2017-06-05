#!/usr/bin/env python

import json
import platform
import os
import shutil
import errno

data = json.load(open('build.json'))

config = data['default']

targetPlatform = ''

# Find and set platform
if platform.system() == 'Linux':
    targetPlatform = 'linux'
elif platform.system() == 'Windows':
    targetPlatform = 'windows'
else:
    raise NameError('Only Linux and Windows are supported')
    exit(1)

# set up config data for the current platform
if 'includes' in data[targetPlatform]:
    config['includes'] = data[targetPlatform]['includes']

if 'build_opts' in data[targetPlatform]:
    config['build_opts'] = data[targetPlatform]['build_opts']

if 'cmake_opts' in data[targetPlatform]:
    config['cmake_opts'] = data[targetPlatform]['cmake_opts']

# Generate cmake includes list
includes = ''
for i in config['includes']:
    includes += i['fullpath'] + ';'

# Generate cmake compiler options
complileOpts = ''
for i in config['compiler_opts']:
    complileOpts += i + ' '

if config['build_opts']['environment'] == '32':
    complileOpts += '-m32'
elif config['build_opts']['environment'] == '64':
    complileOpts += '-m64'

# Generate cmake CMAKE options
cmakeOpts = ''
for key, val in config['cmake_opts'].items():
    cmakeOpts += '-D' + key + '=' + val + ' '

# Generate test executable data
testName = 'test_' + config['build_opts']['environment']
testSrc = ''
for i in config['testSrc']:
    testSrc += i['fullpath'] + ';'


options = (cmakeOpts +
' -DMY_COMPILE_FLAGS="' + complileOpts +
'" -DMY_LINKER_FLAGS="' + complileOpts +
'" -DMY_INCLUDES="' + includes +
'" -DTEST_NAME="' + testName +
'" -DTEST_SOURCES="' + testSrc + '"')

if targetPlatform == 'windows':
    options += ' -G "Unix Makefiles"'

# cd to correct folder
if config['cmake_opts']['CMAKE_BUILD_TYPE'] == 'Debug':
    try:
        os.mkdir('Debug')
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise  # raises the error again
        else:
            shutil.rmtree('Debug')
            os.mkdir('Debug')
            pass

    os.chdir('Debug')

elif config['cmake_opts']['CMAKE_BUILD_TYPE'] == 'Release':
    try:
        os.mkdir('Release')
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise  # raises the error again
        else:
            shutil.rmtree('Release')
            os.mkdir('Release')
            pass

    os.chdir('Release')

else:
    raise NameError('CMAKE_BUILD_TYPE must be Debug or Release')
    exit(1)

# Run console/terminal commands
cmakeCmd = 'cmake '+options+' ..'
print(cmakeCmd)

os.system(cmakeCmd)
os.system('make')

exit(0)
