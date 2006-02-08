#! /usr/bin/env python
## 
# @file  
# win32 related function for several bksys tasks like scanning environment,  
# shared library and .la file creation, creating source packages and 
# project cleaning 
#

# TODO (ce) what about KDE_MAKE_LIB 
                          
## detect win32 specific settings 
def detect(env):
	import os, sys
	import SCons.Util
	env['GENCXXFLAGS'] = []
	env['CPPPATH'] = []

	# (rh) The flags from GENCCFLAGS seems to be added to GENCXXFLAGS, 
	# so there is no need to duplicate settings in GENCXXGLAGS
	if env['ARGS'].has_key('debug'):
		env['BKS_DEBUG'] = env['ARGS'].get('debug')
		env.pprint('CYAN','** Enabling debug for the project **')
		if env['CC'] == 'gcc':
			env['GENCCFLAGS'] = ['-g']
			env['GENLINKFLAGS'] = ['-g']
		elif env['CC'] == 'cl':
			env['GENCCFLAGS'] = ['-Od','-ZI','-MDd']
			env['GENLINKFLAGS'] = ['/INCREMENTAL', '/DEBUG']
	else:
		if env['CC'] == 'gcc':
			env['GENCCFLAGS'] = ['-O2', '-DNDEBUG', '-DNO_DEBUG']
			env['GENLINKFLAGS'] = []
		elif env['CC'] == 'cl':
			env['GENCCFLAGS'] = ['-MD']
			env['GENLINKFLAGS'] = ['']

	if env['CC'] == 'cl':
		# avoid some compiler warnings...
		env.AppendUnique( GENCCFLAGS = ['-wd4619','-wd4820'] )
	
	if os.environ.has_key('CXXFLAGS'):  env['GENCXXFLAGS']  += SCons.Util.CLVar( os.environ['CXXFLAGS'] )
	if os.environ.has_key('CFLAGS'): env['GENCCFLAGS'] = SCons.Util.CLVar( os.environ['CFLAGS'] )
	if os.environ.has_key('LINKFLAGS'): env['GENLINKFLAGS'] += SCons.Util.CLVar( os.environ['LINKFLAGS'] )
	# for make compatibility 
	if os.environ.has_key('LDFLAGS'):   env['GENLINKFLAGS'] += SCons.Util.CLVar( os.environ['LDFLAGS'] )

	# User-specified prefix
	if env['ARGS'].has_key('prefix'):
		env['PREFIX'] = os.path.abspath( env['ARGS'].get('prefix', '') )
		env.pprint('CYAN','** installation prefix for the project set to:',env['PREFIX'])

	# User-specified include paths
	if env['ARGS'].has_key('extraincludes'):
		env['EXTRAINCLUDES'] = env['ARGS'].get('extraincludes','').split(':')
		env.pprint('CYAN','** extra include paths for the project set to:',env['EXTRAINCLUDES'])

	# User-specified library search paths
	if env['ARGS'].has_key('extralibs'):
		env['EXTRALIBS'] = env['ARGS'].get('extralibs','').split(':')
		env.pprint('CYAN','** extra library search paths for the project set to:',env['EXTRALIBS'])

	# no colors if user does not want them
	if os.environ.has_key('NOCOLORS'): env['NOCOLORS']=1

	if env['CC'] == 'cl':
		env.AppendUnique(CPPPATH = [os.environ['INCLUDE'].split()])
		env['GENCCFLAGS']   += ['/Iwin\\include','/Iwin\\include\\msvc']
# TODO (rh) don't know required cl flags, don't know how to set not in win dir 
#		env['GENLINKFLAGS'] += ['']

	elif sys.platform == 'cygwin':
		env['GENCCFLAGS']   += ['-mno-cygwin','-Iwin/include','-Iwin/include/mingw']
		env['GENLINKFLAGS'] += ['-mno-cygwin','-L'+env['_BUILDDIR_']+'\\win']
		# TODO (rh) move to win32 qt4 stuff 
		qtmingwflags = '-DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_EDITION=QT_EDITION_DESKTOP -DQT_DLL -DQT_NO_DEBUG -DQT_CORE_LIB -DQT_GUI_LIB -DQT_THREAD_SUPPORT' + ' -I' + os.environ['QTDIR'] + '/include'
		env['GENCXXFLAGS']  += qtmingwflags.split()

		# required libraries should be installed under mingw installation root, so add the search pathes 
		if os.environ.has_key('MINGW'):  
			env.pprint('CYAN','Checking for mingw installation: ok ',os.environ['MINGW'])
#			env['GENCXXFLAGS']  += ['-I' + os.environ['MINGW'] + '/include']
			env['GENCCFLAGS']   += ['-I' + os.environ['MINGW'] + '/include']
			env['GENLINKFLAGS'] += ['-Wl,--enable-runtime-pseudo-reloc','-Wl,--export-all-symbols','-L' + os.environ['MINGW'] + '/lib']
	elif os.environ.has_key('MINGW'):  
		env.pprint('CYAN','Checking for mingw installation: ok ',os.environ['MINGW'])
		env['GENCCFLAGS']   += ['-Iwin\\include','-Iwin\\include\\mingw']
		env['GENLINKFLAGS'] += ['-L'+env['_BUILDDIR_']+'\\win']

		# required libraries should be installed under mingw installation root, so add the search pathes 
		env['GENCCFLAGS']   += ['-I' + os.environ['MINGW'] + '\\include']
		env['GENLINKFLAGS'] += ['-Wl,--enable-runtime-pseudo-reloc','-Wl,--export-all-symbols','-L' + os.environ['MINGW'] + '\\lib']

		qtmingwflags = '-DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_EDITION=QT_EDITION_DESKTOP -DQT_DLL -DQT_NO_DEBUG -DQT_CORE_LIB -DQT_GUI_LIB -DQT_THREAD_SUPPORT' + ' -I' + os.environ['QTDIR'] + '\\include'
		env['GENCXXFLAGS']  += qtmingwflags.split()
	
## create source package
def dist(env):
	if not version: VERSION=os.popen("cat VERSION").read().rstrip()
	else: VERSION=version
	FOLDER  = appname+'-'+VERSION
	TMPFOLD = ".tmp"+FOLDER
	ARCHIVE = FOLDER+'.tar.bz2'

	# check if the temporary directory already exists
	os.popen('rm -rf %s %s %s' % (FOLDER, TMPFOLD, ARCHIVE) )

	# create a temporary directory
	startdir = os.getcwd()

	os.popen("mkdir -p "+TMPFOLD)	
	os.popen("cp -R * "+TMPFOLD)
	os.popen("mv "+TMPFOLD+" "+FOLDER)

	# remove scons-local if it is unpacked
	os.popen("rm -rf %s/scons %s/sconsign %s/scons-local-0.96.1" % (FOLDER, FOLDER, FOLDER))

	# remove our object files first
	os.popen("find %s -name \"cache\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \"build\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \"*.pyc\" | xargs rm -f " % FOLDER)

	# CVS cleanup
	os.popen("find %s -name \"CVS\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \".cvsignore\" | xargs rm -rf" % FOLDER)

	# Subversion cleanup
	os.popen("find %s -name .svn -type d | xargs rm -rf" % FOLDER)

	# GNU Arch cleanup
	os.popen("find %s -name \"{arch}\" | xargs rm -rf" % FOLDER)
	os.popen("find %s -name \".arch-i*\" | xargs rm -rf" % FOLDER)

	# Create the tarball (coloured output)
	env.pprint('GREEN', 'Writing archive '+ARCHIVE)
	os.popen("tar cjf %s %s " % (ARCHIVE, FOLDER))

	# Remove the temporary directory
	os.popen('rm -rf '+FOLDER)
	env.Exit(0)

## Remove the cache directory
def distclean(env):
	import os, shutil
	if os.path.isdir(env['CACHEDIR']): shutil.rmtree(env['CACHEDIR'])
	os.popen("find . -name \"*.pyc\" | xargs rm -rf")
	env.Exit(0)

## Function for building shared libraries
def bksys_shlib(lenv, ntarget, source, libdir, libprefix='lib', vnum='', noinst=None):
	"""Installs a shared library, with or without a version number	
	note: on win32 platforms only the major version number is used 
	"""
	import types,os,sys
	# parameter can be a list
	if type(ntarget) is types.ListType: target=ntarget[0]
	else: target=ntarget

	thisenv = lenv.Copy() # copying an existing environment is +/- cheap
	thisenv['BKSYS_DESTDIR']=libdir
	thisenv['BKSYS_VNUM']=vnum
	thisenv['SHLIBPREFIX']=libprefix

	if len(vnum)>0:
		if thisenv['CC'] == 'gcc':
			num=vnum.split('.')[0]
			thisenv['SHLIBSUFFIX']='-'+num+'.dll'
	
	lst=target.split(os.sep)
	tname=lst[len(lst)-1]
	libname=tname.split('.')[0]
#	if thisenv['CC'] == 'gcc':
#		thisenv.AppendUnique(LINKFLAGS = ["-Wl,--out-implib=%s.a" % (lenv.join(thisenv['CURBUILDDIR'],libname))] )

	# Fix against a scons bug - shared libs and ordinal out of range(128)
	# (rh) Is this really required ? 
	if type(source) is types.ListType:
		src2=[]
		for i in source: src2.append( str(i) )
		source=src2

	library_list = thisenv.SharedLibrary(target, source)
	lafile_list  = thisenv.LaFile(libprefix+target, library_list)

	# Install the libraries automatically
	if not thisenv.has_key('NOAUTOINSTALL') and not noinst and libdir:
		inst_lib=thisenv.bksys_install(libdir, library_list)
		thisenv.bksys_install(libdir, lafile_list)	

	return library_list

## create .la file used by libtool
def build_la_file(target, source, env):
	""" Writes a .la file, used by libtool """
	dest=open(target[0].path, 'w')
	sname=source[0].name
	dest.write("# Generated by ltmain.sh - GNU libtool 1.5.18 - (pwn3d by bksys)\n#\n#\n")
	if len(env['BKSYS_VNUM'])>0:
		vnum=env['BKSYS_VNUM']
		nums=vnum.split('.')
		src=source[0].name
		name = src.split('-')[0] + '.a'
		dest.write("dlname='%s'\n" % (sname) )
		dest.write("library_names='%s %s'\n" % (sname,name) )
	else:
		dest.write("dlname='%s'\n" % sname)
		src=source[0].name
		name = src.split('.')[0] + '.a'
		dest.write("library_names='%s %s'\n" % (sname, name) )
	dest.write("old_library=''\ndependency_libs=''\ncurrent=0\n")
	dest.write("age=0\nrevision=0\ninstalled=yes\nshouldnotlink=no\n")
	dest.write("dlopen=''\ndlpreopen=''\n")
	dest.write("libdir='%s'" % env['BKSYS_DESTDIR'])
	dest.close()
	#return 0
