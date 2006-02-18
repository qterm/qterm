# Made from scons qt.py and (heavily) modified into kde.py
# Thomas Nagy, 2004, 2005 <tnagy2^8@yahoo.fr>

"""
Run scons -h to display the associated help, or look below ..
"""

import os, re, types

def exists(env):
	return True

def generate(env):
	"""Set up the qt environment and builders - the moc part can be difficult to understand """
	if env['HELP']:
                p=env.pprint
                p('BOLD','*** QT4 options ***')
                p('BOLD','--------------------')
                p('BOLD','* prefix     ','base install path,         ie: /usr/local')
                p('BOLD','* execprefix ','install path for binaries, ie: /usr/bin')
                p('BOLD','* datadir    ','install path for the data, ie: /usr/local/share')
                p('BOLD','* libdir     ','install path for the libs, ie: /usr/lib')
		p('BOLD','* libsuffix  ','suffix of libraries on amd64, ie: 64, 32')
		p('BOLD','* qtincludes ','qt includes path (/usr/include/qt on debian, ..)')
		p('BOLD','* qtlibs     ','qt libraries path, for linking the program')

                p('BOLD','* scons configure libdir=/usr/local/lib qtincludes=/usr/include/qt\n')
		return

	import SCons.Defaults
	import SCons.Tool
	import SCons.Util
	import SCons.Node

	CLVar = SCons.Util.CLVar
	splitext = SCons.Util.splitext
	Builder = SCons.Builder.Builder
	
	# Detect the environment - replaces ./configure implicitely and store the options into a cache
	from SCons.Options import Options
	cachefile=env['CACHEDIR']+'qt4.cache.py'
	opts = Options(cachefile)
	opts.AddOptions(
		('PREFIX', 'root of the program installation'),
		('LIBSUFFIXEXT', 'suffix of libraries on amd64'),

		('QTDIR', ''),
		('QTLIBPATH', 'path to the qt libraries'),
		('QTINCLUDEPATH', 'path to the qt includes'),

		('QT_UIC', 'uic command'),
		('QT_UIC3', 'uic3 command'),
		('QT_MOC', 'moc command'),
		('QT_RCC', 'rcc command'),

		('CPPPATH_QT', ''),
		('LIBPATH_QT', ''),
		('LIB_QT', ''),
		('RPATH_QT', ''),
		('FRAMEWORKPATH_QT', ''),
		('FRAMEWORK_QT', ''),

		('CPPPATH_QT3SUPPORT', ''),
		('CXXFLAGS_QT3SUPPORT', ''),
		('LIB_QT3SUPPORT', ''),
		('RPATH_QT3SUPPORT', ''),
		('FRAMEWORK_QT3SUPPORT', ''),
		('FRAMEWORKPATH_QT3SUPPORT', ''),

		('CPPPATH_QTCORE', ''),
		('LIB_QTCORE', ''),
		('RPATH_QTCORE', ''),
		('FRAMEWORK_QTCORE', ''),
		('FRAMEWORKPATH_QTCORE', ''),

		('CPPPATH_QTDESIGNER', ''),
		('LIB_QTDESIGNER', ''),
		('FRAMEWORK_QTDESIGNER', ''),
		('FRAMEWORKPATH_QTDESIGNER', ''),

		('CPPPATH_QTASSISTANT', ''),
		('LIB_QTASSISTANT', ''),
		('FRAMEWORK_QTASSISTANT', ''),
		('FRAMEWORKPATH_QTASSISTANT', ''),
			
		('CPPPATH_QTNETWORK', ''),
		('LIB_QTNETWORK', ''),
		('RPATH_QTNETWORK', ''),
		('FRAMEWORK_QTNETWORK', ''),
		('FRAMEWORKPATH_QTNETWORK', ''),

		('CPPPATH_QTGUI', ''),
		('LIB_QTGUI', ''),
		('RPATH_QTGUI', ''),
		('FRAMEWORK_QTGUI', ''),
		('FRAMEWORKPATH_QTGUI', ''),

		('CPPPATH_QTOPENGL', ''),
		('LIB_QTOPENGL', ''),
		('RPATH_QTOPENGL', ''),
		('FRAMEWORK_QTOPENGL', ''),
		('FRAMEWORKPATH_QTOPENGL', ''),

		('CPPPATH_QTSQL', ''),
		('LIB_QTSQL', ''),
		('RPATH_QTSQL', ''),
		('FRAMEWORK_QTSQL', ''),
		('FRAMEWORKPATH_QTSQL', ''),

		('CPPPATH_QTXML', ''),
		('LIB_QTXML', ''),
		('RPATH_QTXML', ''),
		('FRAMEWORK_QTXML', ''),
		('FRAMEWORKPATH_QTXML', ''),

		# TODO remove
		('QTPLUGINS', 'uic executable command'),

		('QTLOCALE', 'install po files to this path'),
	)
	opts.Update(env)

	# reconfigure when things are missing
	if not env['HELP'] and (env['_CONFIGURE_'] or (not env.has_key('LIB_QT') and not env.has_key('FRAMEWORK_QT'))):
		env['_CONFIGURE_']=1
		for opt in opts.options:
			if env.has_key(opt.key): env.__delitem__(opt.key)
		import sys
		if sys.platform == 'darwin':
			sys.path.append('bksys'+os.sep+'osx')
			#sys.path.append('bksys'+os.sep+'unix')
			from detect_qt4 import detect
		elif env['WINDOWS']:
			sys.path.append('bksys'+os.sep+'win32')
			from detect_qt4 import detect
		else:
			sys.path.append('bksys'+os.sep+'unix')
			from detect_qt4 import detect
		detect(env)
		opts.Save(cachefile, env)

	## set default variables, one can override them in sconscript files
	env['QT_AUTOSCAN'] = 1
	env['QT_DEBUG']    = 0

	env['MSGFMT']   = 'msgfmt' #TODO whether this should be checked?

	## ui file processing
	def uic_processing(target, source, env):
		comp_h   ='$QT_UIC -o %s %s' % (target[0].path, source[0].path)
		return env.Execute(comp_h)

	def uic3_processing(target, source, env):
		inc_moc  ='#include "%s"\n' % target[2].name
		comp_h   ='$QT_UIC3 -L $QTPLUGINS -nounload -o %s %s' % (target[0].path, source[0].path)
		comp_c   ='$QT_UIC3 -L $QTPLUGINS -nounload -tr tr2i18n -impl %s %s' % (target[0].path, source[0].path)
		comp_moc ='$QT_MOC -o %s %s' % (target[2].path, target[0].path)
		if env.Execute(comp_h):
			return ret
		if env.has_key('UIC3_PRE_INCLUDE'):
			dest = open( target[1].path, "w" )
			dest.write(env['UIC3_PRE_INCLUDE'])
			dest.close()
		if env.Execute( comp_c+" >> "+target[1].path ):
			return ret
		dest = open( target[1].path, "a" )
		dest.write(inc_moc)
		dest.close()
		ret = env.Execute( comp_moc )
		return ret
	def uic3Emitter(target, source, env):
		adjustixes = SCons.Util.adjustixes
		bs = SCons.Util.splitext(str(source[0].name))[0]
		bs = os.path.join(str(target[0].get_dir()),bs)
		target.append(bs+'.cpp')
		target.append(bs+'.moc')
		return target, source

	def uic3_string(target, source, env):
		return "%screating%s %s" % (env['BKS_COLORS']['BLUE'], env['BKS_COLORS']['NORMAL'], target[0].name)
	env['BUILDERS']['Uic']=Builder(action=env.Action(uic_processing, uic3_string),prefix='ui_',suffix='.h',src_suffix='.ui')
	env['BUILDERS']['Uic3']=Builder(action=env.Action(uic3_processing, uic3_string),
		emitter=uic3Emitter,suffix='.h',src_suffix='.ui')

	def qrc_buildit(target, source, env):
		name = SCons.Util.splitext( source[0].name )[0]
		comp='%s -name %s %s -o %s' % (env['QT_RCC'], name, source[0].path, target[0].path)
		return env.Execute(comp)
	def qrc_stringit(target, source, env):
		print "%screating%s %s" % (env['BKS_COLORS']['BLUE'], env['BKS_COLORS']['NORMAL'], target[0].name)

	env['BUILDERS']['Qrc']=Builder(action=env.Action(qrc_buildit, qrc_stringit), suffix='_qrc.cpp', src_suffix='.qrc')

	## MOC processing
	moc_comp   = '$QT_MOC $QT_MOCINCFLAGS -o $TARGET $SOURCE'
	moc_string = "%screating%s $TARGET" % (env['BKS_COLORS']['BLUE'], env['BKS_COLORS']['NORMAL'])
	if not env['_USECOLORS_']: moc_string=""
	moc_action = env.Action(moc_comp, moc_string)

	env['BUILDERS']['Moc']    = Builder(action=moc_action,suffix='.moc',src_suffix='.h')
	env['BUILDERS']['Moccpp'] = Builder(action=moc_action,suffix='_moc.cpp',src_suffix='.h')
	# for Moc2 you have to give the dependency explicitely, eg: env.Moc2(['file.cpp'])
	env['BUILDERS']['Moc2']   = Builder(action=moc_action,suffix='.moc',src_suffix='.cpp')
	# we need an extra option because moc only likes '-I ...' and not '/I ...' 
	env['QT_MOCINCFLAGS']     = '$( ${_concat("-I", CPPPATH, "", __env__, RDirs, TARGET, SOURCE)} $)'

	## TRANSLATIONS
	#env['BUILDERS']['Transfiles']=Builder(action='$MSGFMT $SOURCE -o $TARGET',suffix='.gmo',src_suffix='.po')

	## Handy helpers for building kde programs
	## You should not have to modify them ..

	ui_ext = [".ui"]
	ui3_ext = [".ui3"]
	header_ext = [".h", ".hxx", ".hpp", ".hh"]
	cpp_ext = [".cpp", ".cxx", ".cc"]

	def QT4files(lenv, target, source):
		""" Returns a list of files for scons (handles qt4 tricks like .qrc) """
		q_object_search = re.compile(r'\n\s*Q_OBJECT[^A-Za-z0-9]')
		def scan_moc(cppfile):
			addfile=None

			# try to find the header
			orifile=cppfile.srcnode().name
			bs=SCons.Util.splitext(orifile)[0]

			h_file=''
			dir=cppfile.dir
			for n_h_ext in header_ext:
				afile=dir.File(bs+n_h_ext)
				if afile.rexists():
					#h_ext=n_h_ext
					h_file=afile
					break
			# We have the header corresponding to the cpp file
			if h_file:
				h_contents = h_file.get_contents()
				if q_object_search.search(h_contents):
					# we know now there is Q_OBJECT macro
					reg = '\n\s*#include\s*("|<)'+str(bs)+'.moc("|>)'
					meta_object_search = re.compile(reg)
					#cpp_contents = open(file_cpp, 'rb').read()
					cpp_contents=cppfile.get_contents()
					if meta_object_search.search(cpp_contents):
						lenv.Moc(h_file)
					else:
						lenv.Moccpp(h_file)
						addfile=bs+'_moc.cpp'
						print "WARNING: moc.cpp for "+h_file.name+" consider using #include <file.moc> instead"
			return addfile


		src=[]
		ui_files=[]
		other_files=[]

		source_=lenv.make_list(source)

		# For each file, check wether it is a dcop file or not, and create the complete list of sources
		for file in source_:
			sfile=SCons.Node.FS.default_fs.File(str(file))
			bs  = SCons.Util.splitext(file)[0]
			ext = SCons.Util.splitext(file)[1]
			if ext == ".moch":
				lenv.Moccpp(bs+'.h')
				src.append(bs+'_moc.cpp')
			elif ext == ".qrc":
				src.append(bs+'_qrc.cpp')
				lenv.Qrc(file)
				
				from xml.sax import make_parser
				from xml.sax.handler import ContentHandler

				class SconsHandler(ContentHandler):
				        def __init__ (self, envi):
						self.files=[]
						self.mstr=''
					def startElement(self, name, attrs):
						if name =='file':
							self.mstr=''
					def endElement(self, name):
						if name =='file':
							self.files.append(self.mstr)
							self.mstr=''
					def characters(self, chars):
						self.mstr+=chars

				parser = make_parser()
				curHandler = SconsHandler(lenv)
				parser.setContentHandler(curHandler)
				parser.parse(open(sfile.srcnode().abspath))
				files=curHandler.files

				lenv.Depends(bs+'_qrc.cpp', files)
				#for i in files:
				#	print "   -> "+i
			elif ext in cpp_ext:
				src.append(file)
				if not env.has_key('NOMOCFILE'):
					ret = scan_moc(sfile)
					if ret: src.append( ret )
			elif ext in ui_ext:
				lenv.Uic(file)
			elif ext in ui3_ext:
				lenv.Uic3(bs+'.ui')
				src.append(bs+'.cpp')
			else:
				src.append(file)

		# Now check against typical newbie errors
		for file in ui_files:
			for ofile in other_files:
				if ofile == file:
					lenv.pprint('RED', "WARNING: You have included "+file+".ui and another file of the same prefix")
					print "Files generated by uic (file.h, file.cpp must not be included"
		return src


	""" In the future, these functions will contain the code that will dump the
	configuration for re-use from an IDE """
	import glob

	def QT4install(lenv, restype, subdir, files):
		if not env['_INSTALL_']:
			return
		dir = getInstDirForResType(lenv, restype)
		install_list = lenv.bksys_install(lenv.join(dir,subdir), files, nodestdir=1)
		return install_list

	def QT4installas(lenv, restype, destfile, file):
		if not env['_INSTALL_']:
			return
		dir = getInstDirForResType(lenv, restype)
		install_list = lenv.InstallAs(lenv.join(dir,destfile), file)
                env.Alias('install', install_list)
		return install_list

	def QT4lang(lenv, folder, appname):
		""" Process translations (.po files) in a po/ dir """
		transfiles = glob.glob(folder+'/*.po')
		for lang in transfiles:
			result = lenv.Transfiles(lang)
			country = SCons.Util.splitext(result[0].name)[0]
			lenv.QT4installas('QTLOCALE', lenv.join(country,'LC_MESSAGES',appname+'.mo'), result)

	import generic
	class qt4obj(generic.genobj):
		def __init__(self, val, senv=None):
			if senv: generic.genobj.__init__(self, val, senv)
			else: generic.genobj.__init__(self, val, env)
			if val == 'shlib':     self.instdir=senv.join(senv['PREFIX'], 'lib')
			elif val == 'program': self.instdir=senv.join(senv['PREFIX'], 'bin')
		def execute(self):
			if self.executed: return
			if self.orenv.has_key('DUMPCONFIG'):
				print self.xml()
				self.executed=1
				return
			self.env=self.orenv.Copy()
			#self.env2=self.orenv.Copy()
			self.setsource( QT4files(self.env, self.target, self.source) )
			generic.genobj.execute(self)
		def xml(self):
			ret= '<compile type="%s" dirprefix="%s" target="%s" cxxflags="%s" cflags="%s" includes="%s" linkflags="%s" libpaths="%s" libs="%s" vnum="%s" iskdelib="%s" libprefix="%s">\n' % (self.type, self.dirprefix, self.target, self.cxxflags, self.cflags, self.includes, self.linkflags, self.libpaths, self.libs, self.vnum, self.iskdelib, self.libprefix)
			if self.source:
				for i in self.orenv.make_list(self.source):
					ret += '  <source file="%s"/>\n' % i
			ret += "</compile>"
			return ret

	# Attach the functions to the environment so that SConscripts can use them
	from SCons.Script.SConscript import SConsEnvironment
	SConsEnvironment.QT4install = QT4install
	SConsEnvironment.QT4installas = QT4installas
	SConsEnvironment.QT4lang = QT4lang
	SConsEnvironment.qt4obj=qt4obj

