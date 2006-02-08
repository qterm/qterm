#! /usr/bin/env python

"""
help       -> scons -h
compile    -> scons
clean      -> scons -c
install    -> scons install
uninstall  -> scons -c install
configure  -> scons configure debug=full extraincludes=/usr/local/include:/tmp/include prefix=/usr/local

Run from a subdirectory -> scons -u (or export SCONSFLAGS="-u" in your ~/.bashrc)

Documentation is in bksys/design
"""

# QUICKSTART (or look in bksys/design)
# cd kdelibs
# tar xjvf bksys/scons-mini.tar.bz2
# ./scons

# Suggested plan ? (ita)
# * convert some kde4 code as a base for further developments : ok
# * find the variables for the configuration modules          : ok
# * create the config.h system                                : ok
# * write the configuration modules                           : ~
# * fix the installation process and the kobject builder      :
# * use an automatic converter for the rest of the tree       :

###################################################################
# LOAD THE ENVIRONMENT AND SET UP THE TOOLS
###################################################################

## We assume that 'bksys' is our admin directory
import sys, os
sys.path.append('bksys')

## Import the main configuration tool
from generic import configure
config = {
	'modules'  : 'generic qt4',
	'builddir' : 'build', # put all object files under 'build/'
	'config.h' : 0,
	'rpath'    : 1,
	'bootstrap': 1,
	'colorful' : not os.environ.has_key('NOCOLORS'),
}

# and the config.h
env=configure(config)

###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################
subdirs="""
src
"""

"""
# TODO this will not stay like this ..
dirs=[] # the dirs to process are appended to this var in the loop below
for dir in subdirs.split():
	jdir =  env.join('#build', dir)
	try:
		os.mkdir(jdir[1:])
	except OSError:
		pass
	env.BuildDir( jdir, dir, duplicate=0)
	dirs.append( jdir )
env.subdirs(dirs)
"""

env.BuildDir('#build/src', 'src', duplicate=0)
env.subdirs("build/src")

###################################################################
# CONVENIENCE FUNCTIONS TO EMULATE 'make dist' and 'make distclean'
###################################################################

# if you forget to add a version number, the one in the file VERSION will be used instead
env.dist('bksys-qt4', '1.0.1')

