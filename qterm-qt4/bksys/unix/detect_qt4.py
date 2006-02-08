# Thomas Nagy, 2005


import os, re, types, sys

def detect(env):
	def getpath(varname):
		if not env.has_key('ARGS'): return None
		v=env['ARGS'].get(varname, None)
		if v : v=os.path.abspath(v)
		return v

	prefix		= getpath('prefix')
	execprefix	= getpath('execprefix')
	datadir		= getpath('datadir')
	libdir		= getpath('libdir')
	qtincludes	= getpath('qtincludes')
	qtlibs		= getpath('qtlibs')
	libsuffix	= ''
	if env.has_key('ARGS'): libsuffix=env['ARGS'].get('libsuffix', '')
	if not libsuffix:
		if sys.platform == 'darwin':
			libsuffix='.dylib'
		else:
			libsuffix='.so'

	p=env.pprint

	# do our best to find the QTDIR (non-Debian systems)
	qtdir = os.getenv("QTDIR")
	if qtdir and env.find_file('lib/libqt-mt' + libsuffix, [qtdir]): qtdir=None # qtdir for qt3, not qt4
	if not qtdir:
		qtdir=env.find_path('include/', [ # lets find the qt include directory
				'/usr/local/Trolltech/Qt-4.0.3/', # one never knows
				'/usr/local/Trolltech/Qt-4.0.2/',
				'/usr/local/Trolltech/Qt-4.0.1/',
				'/usr/local/Trolltech/Qt-4.0.0/'])
		if qtdir: p('YELLOW', 'The qtdir was found as '+qtdir)
		else:     p('YELLOW', 'There is no QTDIR set')
	else: env['QTDIR'] = qtdir.strip()

	# if we have the QTDIR, finding the qtlibs and qtincludes is easy
	if qtdir:
		if not qtlibs:     qtlibs     = os.path.join(qtdir, 'lib')
		if not qtincludes: qtincludes = os.path.join(qtdir, 'include')
		#os.putenv('PATH', os.path.join(qtdir , 'bin') + ":" + os.getenv("PATH")) # TODO ita 

	# Check for uic, uic-qt3, moc, rcc, ..
	def find_qt_bin(progs):
		# first use the qtdir
		path=''
		for prog in progs:
			path=env.find_program(prog, [env.join(qtdir, 'bin')])
			if path:
				p('GREEN',"%s was found as %s" % (prog, path))
				return path

		if not env['WINDOWS']: #(js) temp. check
			# else use the environment
			for prog in progs:
				path=env.find_program_using_which(prog)
				if path:
					p('YELLOW',"%s was found as %s" % (prog, path))
					return path
			
			# and then try to guess using common paths ..
			common_paths=['/usr/bin', '/usr/local/bin', '/opt/bin', '/opt/local/bin']
			for prog in progs:
				path=env.find_program(prog, common_paths)
				if path:
					p('YELLOW',"%s was found as %s" % (prog, path))
					return path

		# everything failed
		p('RED',"%s was not found - make sure Qt4-devel is installed, or set $QTDIR or $PATH" % prog)
		env.Exit(1)
	
	print "Checking for uic               :",
	env['QT_UIC']  = find_qt_bin(['uic', 'uic-qt4'])

	print "Checking for uic-qt3           :",
	env['QT_UIC3'] = find_qt_bin(['uic3', 'uic-qt3'])

	print "Checking for moc               :",
	env['QT_MOC'] = find_qt_bin(['moc', 'moc-qt4'])

	print "Checking for rcc               :",
	env['QT_RCC'] = find_qt_bin(['rcc'])

	# TODO is this really needed now ?
	print "Checking for uic3 version      :",
	version = os.popen(env['QT_UIC'] + " -version 2>&1").read().strip()
	if version.find(" 3.") != -1:
		version = version.replace('Qt user interface compiler','')
		version = version.replace('User Interface Compiler for Qt', '')
		p('RED', version + " (too old)")
		env.Exit(1)
	p('GREEN', "fine - %s" % version)

	#if os.environ.has_key('PKG_CONFIG_PATH'):
	#	os.environ['PKG_CONFIG_PATH'] = os.environ['PKG_CONFIG_PATH'] + ':' + qtlibs
	#else:
	#	os.environ['PKG_CONFIG_PATH'] = qtlibs

	## check for the Qt4 includes
	print "Checking for the Qt4 includes  :",
	if qtincludes and os.path.isfile(qtincludes + "/QtGui/QFont"):
		# The user told where to look for and it looks valid
		p('GREEN','ok '+qtincludes)
	else:
		if os.path.isfile(qtdir+'/include/QtGui/QFont'):
			# Automatic detection
			p('GREEN','ok '+qtdir+"/include/")
			qtincludes = qtdir + "/include/"
		elif os.path.isfile("/usr/include/qt4/QtGui/QFont"):
			# Debian probably
			p('YELLOW','the qt headers were found in /usr/include/qt4/')
			qtincludes = "/usr/include/qt4"
		else:
			p('RED',"the qt headers were not found")
			env.Exit(1)


	## TODO make this specific to the the module QT4
	if not prefix: prefix = "/usr/"
		
	## use the user-specified prefix
	if not execprefix: execprefix = prefix
	if not datadir:    datadir    = prefix+"/share"
	if not libdir:     libdir     = execprefix+"/lib"

	subst_vars = lambda x: x.replace('${exec_prefix}', execprefix)\
			 .replace('${datadir}', datadir)\
			 .replace('${libdir}', libdir)
	debian_fix = lambda x: x.replace('/usr/share', '${datadir}')

	env['PREFIX'] = prefix
	env['LIBSUFFIXEXT'] = libsuffix

	#env['QTPLUGINS']=os.popen('kde-config --expandvars --install qtplugins').read().strip()

	## qt libs and includes
	env['QTINCLUDEPATH']=qtincludes
	if not qtlibs: qtlibs=env.join(qtdir, 'lib')
	env['QTLIBPATH']=qtlibs

        ########## X11
        env['LIB_X11']             = ['X11']
        env['LIBPATH_X11']         = ['/usr/X11R6/lib/']
        env['LIB_XRENDER']         = ['Xrender']

	debug=""
	if env.has_key('BKS_DEBUG'):
		if env['BKS_DEBUG']:
			debug='_debug'

        ########## QT
        # QTLIBPATH is a special var used in the qt4 module - has to be changed (ita)
	env['CPPPATH_QT']          = [ env.join(env['QTINCLUDEPATH'], 'Qt'), env['QTINCLUDEPATH'] ] # TODO QTINCLUDEPATH (ita)
        env['LIBPATH_QT']          = env['LIBPATH_X11']+[env['QTLIBPATH']]
        env['LIB_QT']              = ['QtGui'+debug, 'pthread', 'Xext']+env['LIB_X11']#+env['LIB_PNG']+env['LIB_SM']
	env['RPATH_QT']            = env['LIBPATH_X11']+[env['QTLIBPATH']]

        env['CXXFLAGS_QT3SUPPORT'] = ['-DQT3_SUPPORT']
	env['CPPPATH_QT3SUPPORT']  = [ env.join(env['QTINCLUDEPATH'], 'Qt3Support') ]
        env['LIB_QT3SUPPORT']      = ['Qt3Support'+debug]
	env['RPATH_QT3SUPPORT']    = env['RPATH_QT']

	env['CPPPATH_QTCORE']      = [ env.join(env['QTINCLUDEPATH'], 'QtCore') ]
        env['LIB_QTCORE']          = ['QtCore'+debug]
	env['RPATH_QTCORE']        = env['RPATH_QT']

	env['CPPPATH_QTASSISTANT'] = [ env.join(env['QTINCLUDEPATH'], 'QtAssistant') ]
	env['LIB_QTASSISTANT']     = ['QtAssistant'+debug]

	env['CPPPATH_QTDESIGNER']  = [ env.join(env['QTINCLUDEPATH'], 'QtDesigner') ]
        env['LIB_QTDESIGNER']      = ['QtDesigner'+debug]

	env['CPPPATH_QTNETWORK']   = [ env.join(env['QTINCLUDEPATH'], 'QtNetwork') ]
        env['LIB_QTNETWORK']       = ['QtNetwork'+debug]
	env['RPATH_QTNETWORK']     = env['RPATH_QT']

	env['CPPPATH_QTGUI']       = [ env.join(env['QTINCLUDEPATH'], 'QtGui') ]
        env['LIB_QTGUI']           = ['QtCore'+debug, 'QtGui'+debug]
	env['RPATH_QTGUI']         = env['RPATH_QT']

	env['CPPPATH_OPENGL']      = [ env.join(env['QTINCLUDEPATH'], 'QtOpengl') ]
        env['LIB_QTOPENGL']        = ['QtOpenGL'+debug]
	env['RPATH_QTOPENGL']      = env['RPATH_QT']

	env['CPPPATH_QTSQL']       = [ env.join(env['QTINCLUDEPATH'], 'QtSql') ]
        env['LIB_QTSQL']           = ['QtSql'+debug]
	env['RPATH_QTSQL']         = env['RPATH_QT']

	env['CPPPATH_QTXML']       = [ env.join(env['QTINCLUDEPATH'], 'QtXml') ]
        env['LIB_QTXML']           = ['QtXml'+debug]
	env['RPATH_QTXML']         = env['RPATH_QT']
	
	env['QTLOCALE']=env.join(datadir, 'locale')


