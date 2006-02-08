# Thomas Nagy, 2005 <tnagy2^8@yahoo.fr>


import os, re, types

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

	p=env.pprint

	qtdir = os.getenv("QTDIR")
	if not qtdir:
		p('RED', 'QTDIR not set - please set it')
		env.Exit(1)
	env['QTDIR'] = qtdir.strip()
	if qtdir:
		if not qtlibs:
			qtlibs = os.path.join(qtdir, 'lib' + libsuffix)
		if not qtincludes:
			qtincludes = os.path.join(qtdir, 'include')
		os.putenv('PATH', os.path.join(qtdir , 'bin') +
				  ":" + os.getenv("PATH"))

	def find_qt_bin(prog):
		## Find the necessary programs
		print "Checking for %s               :" % prog,
		path = os.path.join(qtdir, 'bin', prog)
		if os.path.isfile(path):
			p('GREEN',"%s was found as %s" % (prog, path))
		else:
			path = os.popen("which %s 2>/dev/null" % prog).read().strip()
			if len(path):
				p('YELLOW',"%s was found as %s" % (prog, path))
			else:
				path = os.popen("which %s 2>/dev/null" % prog).read().strip()
				if len(path):
					p('YELLOW',"%s was found as %s" % (prog, path))
				else:
					p('RED',"%s was not found - fix $QTDIR or $PATH" % prog)
					env.Exit(1)
		return path
	
	env['QT_UIC'] = find_qt_bin('uic')
	env['QT_UIC3'] = find_qt_bin('uic3')
	print "Checking for uic version       :",
	version = os.popen(env['QT_UIC'] + " -version 2>&1").read().strip()
	if version.find(" 3.") != -1:
		version = version.replace('Qt user interface compiler','')
		version = version.replace('User Interface Compiler for Qt', '')
		p('RED', version + " (too old)")
		env.Exit(1)
	p('GREEN', "fine - %s" % version)
	env['QT_MOC'] = find_qt_bin('moc')
	env['QT_RCC'] = find_qt_bin('rcc')

	if os.environ.has_key('PKG_CONFIG_PATH'):
		os.environ['PKG_CONFIG_PATH'] = os.environ['PKG_CONFIG_PATH'] \
										+ ':' + qtlibs
	else:
		os.environ['PKG_CONFIG_PATH'] = qtlibs

	## check for the qt and kde includes
	print "Checking for the qt includes   :",
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

	if prefix:
		## use the user-specified prefix
		if not execprefix:
			execprefix = prefix
		if not datadir:
			datadir=prefix+"/share"
		if not libdir:
			libdir=execprefix+"/lib"+libsuffix

		subst_vars = lambda x: x.replace('${exec_prefix}', execprefix)\
				 .replace('${datadir}', datadir)\
				 .replace('${libdir}', libdir)
		debian_fix = lambda x: x.replace('/usr/share', '${datadir}')
		env['PREFIX'] = prefix
	else:
		env['PREFIX'] = "/usr/"

	#env['QTPLUGINS']=os.popen('kde-config --expandvars --install qtplugins').read().strip()

	## qt libs and includes
	env['QTINCLUDEPATH']=qtincludes
	if not qtlibs: qtlibs=env.join(qtdir, 'lib', libsuffix)
	env['QTLIBPATH']=qtlibs

