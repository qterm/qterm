#! /usr/bin/env python

def detect(env):
	import os, sys
	import SCons.Util

	if env['ARGS'].get('debug', None):
		env['BKS_DEBUG'] = env['ARGS'].get('debug', None)
		env.pprint('CYAN','** Enabling debug for the project **')
		env['GENCXXFLAGS'] = ['-g']
		env['GENCCFLAGS'] = ['-g']
		env['GENLINKFLAGS'] = ['-g']
	else:
		env['GENCXXFLAGS'] = ['-O2', '-DNDEBUG', '-DNO_DEBUG']
		env['GENCCFLAGS'] = ['-O2', '-DNDEBUG', '-DNO_DEBUG']
		env['GENLINKFLAGS'] = []
	
	if os.environ.has_key('CXXFLAGS'):  env['GENCXXFLAGS']  += SCons.Util.CLVar( os.environ['CXXFLAGS'] )
	if os.environ.has_key('CFLAGS'): env['GENCCFLAGS'] = SCons.Util.CLVar( os.environ['CFLAGS'] )
	if os.environ.has_key('LINKFLAGS'): env['GENLINKFLAGS'] += SCons.Util.CLVar( os.environ['LINKFLAGS'] )
	# for make compatibility 
	if os.environ.has_key('LDFLAGS'):   env['GENLINKFLAGS'] += SCons.Util.CLVar( os.environ['LDFLAGS'] )

	## Mac OS X settings
	if sys.platform == 'darwin':
		env['GENCXXFLAGS'] += ['-fno-common']
		#env['GENLINKFLAGS'] += ['-undefined', 'dynamic_lookup']

	## Linux settings
	if sys.platform == 'linux2':
		env['GENCXXFLAGS'] += ['-D_XOPEN_SOURCE=500', '-D_BSD_SOURCE', '-D_GNU_SOURCE']

	## FreeBSD settings (contributed by will at freebsd dot org)
	if os.uname()[0] == "FreeBSD":
		if os.environ.has_key('PTHREAD_LIBS'):
			env.AppendUnique( GENLINKFLAGS = SCons.Util.CLVar( os.environ['PTHREAD_LIBS'] ) )
		else:
			syspf = os.popen('/sbin/sysctl kern.osreldate')
			osreldate = int(syspf.read().split()[1])
			syspf.close()
			if osreldate < 500016:
				env.AppendUnique( GENLINKFLAGS = ['-pthread'])
				env.AppendUnique( GENCXXFLAGS = ['-D_THREAD_SAFE'])
			elif osreldate < 502102:
				env.AppendUnique( GENLINKFLAGS = ['-lc_r'])
				env.AppendUnique( GENCXXFLAGS = ['-D_THREAD_SAFE'])
			else:
				env.AppendUnique( GENLINKFLAGS = ['-pthread'])

		env.AppendUnique( GENLINKFLAGS = ['-L/usr/local/lib', '-L/usr/X11R6/lib'] )
		env.AppendUnique( GENCXXFLAGS = ['-I/usr/local/include', '-I/usr/X11R6/include'] )

	# User-specified prefix
	if env['ARGS'].has_key('prefix'):
		env['PREFIX'] = os.path.abspath( env['ARGS'].get('prefix', '') )
		env.pprint('CYAN','** installation prefix for the project set to:',env['PREFIX'])

	# User-specified include paths
	env['EXTRAINCLUDES'] = env['ARGS'].get('extraincludes', None)
	if env['EXTRAINCLUDES']:
		env['EXTRAINCLUDES'] = env['EXTRAINCLUDES'].split(':')
		env.pprint('CYAN','** extra include paths for the project set to:',env['EXTRAINCLUDES'])

	# User-specified library search paths
	env['EXTRALIBS'] = env['ARGS'].get('extralibs', None)
	if env['EXTRALIBS']:
		env['EXTRALIBS'] = env['EXTRALIBS'].split(':')
		env.pprint('CYAN','** extra library search paths for the project set to:',env['EXTRALIBS'])

	## no colors if user does not want them
	if os.environ.has_key('NOCOLORS'): env['NOCOLORS']=1



def dist(env, appname, version=''):
	import os
	if not version: VERSION=os.popen("cat VERSION").read().rstrip()
	else: VERSION=version
	FOLDER  = appname+'-'+VERSION
	TMPFOLD = ".tmp"+FOLDER
	ARCHIVE = FOLDER+'.tar.bz2'

	## check if the temporary directory already exists
	os.popen('rm -rf %s %s %s' % (FOLDER, TMPFOLD, ARCHIVE) )

	## create a temporary directory
	startdir = os.getcwd()

	os.popen("mkdir -p "+TMPFOLD)	
	os.popen("cp -R * "+TMPFOLD)
	os.popen("mv "+TMPFOLD+" "+FOLDER)

	## remove scons-local if it is unpacked
	os.popen("rm -rf %s/scons %s/sconsign %s/scons-local-0.96*" % (FOLDER, FOLDER, FOLDER))

	## remove our object files first
	os.popen("find %s -name \"cache\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \"build\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \"*.pyc\" | xargs rm -f " % FOLDER)

	## CVS cleanup
	os.popen("find %s -name \"CVS\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \".cvsignore\" | xargs rm -rf" % FOLDER)

	## Subversion cleanup
	os.popen("find %s -name .svn -type d | xargs rm -rf" % FOLDER)

	## GNU Arch cleanup
	os.popen("find %s -name \"{arch}\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \".arch-i*\" | xargs rm -rf" % FOLDER)

	## Create the tarball (coloured output)
	env.pprint('GREEN', 'Writing archive '+ARCHIVE)
	os.popen("tar cjf %s %s " % (ARCHIVE, FOLDER))

	## Remove the temporary directory
	os.popen('rm -rf '+FOLDER)
	env.Exit(0)

def distclean(env):
	## Remove the cache directory
	import os, shutil
	if os.path.isdir(env['CACHEDIR']): shutil.rmtree(env['CACHEDIR'])
	os.popen("find . -name \"*.pyc\" | xargs rm -rf")
	env.Exit(0)

