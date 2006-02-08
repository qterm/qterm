#! /usr/bin/env python

import re

def exists(env):
	return True

def generate(env):
	if env['HELP']: return

	from SCons.Tool import Tool
	Tool('qt4', ['./bksys']).generate(env)

	from SCons.Options import Options
	cachefile=env['CACHEDIR']+'kde4.cache.py'
	opts = Options(cachefile)
	opts.AddOptions(
		('KDE4_ISCONFIGURED', ''),

		('KDEDIR', ''),
		('KDELIBPATH', 'path to the installed kde libs'),
		('KDEINCLUDEPATH', 'path to the installed kde includes'),
		('KDECCFLAGS', 'kde-related C compiler flags'),
		('KDECXXFLAGS', 'kde-related c++ compiler flags'),

		('LIBPATH_KDE4', ''),
		('RPATH_KDE4', ''),
		('INCLUDES_KDE4', ''),

		('KDEBIN', 'inst path of the kde binaries'),
		('KDEINCLUDE', 'inst path of the kde include files'),
		('KDELIB', 'inst path of the kde libraries'),
		('KDEMODULE', 'inst path of the parts and libs'),
		('KDEDATA', 'inst path of the application data'),
		('KDELOCALE', ''), ('KDEDOC', ''), ('KDEKCFG', ''),
		('KDEXDG', ''), ('KDEXDGDIR', ''), ('KDEMENU', ''),
		('KDEMIME', ''), ('KDEICONS', ''), ('KDESERV', ''),
		('KDESERVTYPES', ''), ('KDEAPPS', ''), ('KDECONF',''),
		
		# the main KDE libraries - the naming is important here, see genobj.uselib
		('LIBPATH_KDECORE', ''),
		('LIBPATH_KIO', ''),
		('LIBPATH_KDEUI', ''),
		('LIBPATH_QTTEST', ''),

		('CPPPATH_KDECORE', ''),
		('CPPPATH_KIO', ''),
		('CPPPATH_KDEUI', ''),
		('CPPPATH_QTTEST', ''),

		('LIB_KDECORE', ''),
		('LIB_KIO', ''),
		('LIB_KDEUI', ''),
		('LIB_QTTEST', ''),

		('KCONFIGCOMPILER', ''),
		('DCOPIDL', ''), ('DCOPIDL2CPP', ''),
		('UIC3_PRE_INCLUDE', '')
	)
	opts.Update(env)

	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('KDE4_ISCONFIGURED')):
		env['_CONFIGURE_']=1
		import sys
		import os
		if env['WINDOWS']: 
			sys.path.append('bksys'+os.sep+'win32')
			from detect_kde4 import detect
		else:
			sys.path.append('bksys'+os.sep+'unix')
			from detect_kde4 import detect
		detect(env)

		dest=open(env.join(env['_BUILDDIR_'], 'config-kde.h'), 'w')
		dest.write('/* kde configuration */\n')
		dest.write(('#define KDELIBSUFF "%s"\n') % env['LIBSUFFIXEXT']);
		dest.close()
		env['_CONFIG_H_'].append('kde')
		
		env['KDE4_ISCONFIGURED']=1
		env['UIC3_PRE_INCLUDE']="#include <kdialog.h>\n#include <klocale.h>\n"

		# (js) update here
		opts.Save(cachefile, env)
		
	import SCons.Defaults
	Builder=SCons.Builder.Builder

	creation_string = "%screating%s $TARGET.path" % (env['BKS_COLORS']['BLUE'], env['BKS_COLORS']['NORMAL'])

	## KIDL file
	if env['WINDOWS']:
		kidl_str='$DCOPIDL $SOURCE $TARGET'
	else:
		kidl_str='$DCOPIDL $SOURCE > $TARGET || (rm -f $TARGET ; false)'
	if not env['_USECOLORS_']: creation_string=""
	kidl_action=env.Action(kidl_str, creation_string)
	env['BUILDERS']['Kidl']=Builder(action=kidl_action, suffix='.kidl', src_suffix='.h')

	## TODO --no-signals (ita)

	## DCOP
	dcop_str='$DCOPIDL2CPP --c++-suffix cpp --no-stub $SOURCE'
	dcop_action=env.Action(dcop_str, creation_string)

        def dcopEmitter(target, source, env):
		import os
                adjustixes = SCons.Util.adjustixes
                bs = SCons.Util.splitext(str(source[0].name))[0]
                bs = os.path.join(str(target[0].get_dir()),bs)
                #target.append(bs+'_skel.h') # not necessary for dcop, right ?
                return target, source

	env['BUILDERS']['Dcop']=Builder(action=dcop_action, suffix='_skel.cpp', src_suffix='.kidl', emitter=dcopEmitter)

	## STUB
	stub_str='$DCOPIDL2CPP --c++-suffix cpp --no-skel $SOURCE'
	stub_action=env.Action(stub_str, creation_string)

        def stubEmitter(target, source, env):
		import os
                adjustixes = SCons.Util.adjustixes
                bs = SCons.Util.splitext(str(source[0].name))[0]
                bs = os.path.join(str(target[0].get_dir()),bs)
                target.append(bs+'_stub.h')
                return target, source

	env['BUILDERS']['Stub']=Builder(action=stub_action, suffix='_stub.cpp', src_suffix='.kidl', emitter=stubEmitter)

	## DOCUMENTATION
	env['BUILDERS']['Meinproc']=Builder(action='$MEINPROC --check --cache $TARGET $SOURCE',suffix='.cache.bz2')

	def kcfg_buildit(target, source, env):
		comp='$KCONFIGCOMPILER -d%s %s %s' % (str(target[0].get_dir()), source[1].path, source[0].path)
		return env.Execute(comp)
	
	def kcfg_stringit(target, source, env):
		print "processing %s to get %s and %s" % (source[0].name, target[0].name, target[1].name)
		
	def kcfgEmitter(target, source, env):
		adjustixes = SCons.Util.adjustixes
		bs = SCons.Util.splitext(str(source[0].name))[0]
		bs = os.path.join(str(target[0].get_dir()),bs)
		# .h file is already there
		target.append(bs+'.cpp')

		if not os.path.isfile(str(source[0])):
			lenv.pprint('RED','kcfg file given'+str(source[0])+' does not exist !')
			return target, source
		kfcgfilename=""
		kcfgFileDeclRx = re.compile("^[fF]ile\s*=\s*(.+)\s*$")
		for line in file(str(source[0]), "r").readlines():
			match = kcfgFileDeclRx.match(line.strip())
			if match:
				kcfgfilename = match.group(1).strip()
				break
		if not kcfgfilename:
			print 'invalid kcfgc file'
			return 0
		source.append(  env.join(str(source[0].get_dir()), kcfgfilename)  )
		return target, source

	env['BUILDERS']['Kcfg']=Builder(action=env.Action(kcfg_buildit,
													  kcfg_stringit),
									emitter=kcfgEmitter,
									suffix='.h', src_suffix='.kcfgc')


	def KDEicon(lenv, icname='*', path='./', restype='KDEICONS', subdir=''):
		"""Installs icons with filenames such as cr22-action-frame.png into
		KDE icon hierachy with names like icons/crystalsvg/22x22/actions/frame.png.
		
		Global KDE icons can be installed simply using env.KDEicon('name').
		The second parameter, path, is optional, and specifies the icons
		location in the source, relative to the SConscript file.
		
		To install icons that need to go under an applications directory (to
		avoid name conflicts, for example), use e.g.
		env.KDEicon('name', './', 'KDEDATA', 'appname/icons')"""
		
		if lenv.has_key('DUMPCONFIG'):
			lenv.add_dump( "<icondir>\n" )
			lenv.add_dump( "    <icondirent dir=\"%s\" subdir=\"%s\"/>\n" % (reldir(path), subdir) )
			lenv.add_dump( "</icondir>\n" )
			return

		type_dic = { 'action':'actions', 'app':'apps', 'device':'devices',
					 'filesys':'filesystems', 'mime':'mimetypes' }
		dir_dic = {
			'los'  :'locolor/16x16', 'lom'  :'locolor/32x32',
			'him'  :'hicolor/32x32', 'hil'  :'hicolor/48x48',
			'lo16' :'locolor/16x16', 'lo22' :'locolor/22x22', 'lo32' :'locolor/32x32',
			'hi16' :'hicolor/16x16', 'hi22' :'hicolor/22x22', 'hi32' :'hicolor/32x32',
			'hi48' :'hicolor/48x48', 'hi64' :'hicolor/64x64', 'hi128':'hicolor/128x128',
			'hisc' :'hicolor/scalable',
			'cr16' :'crystalsvg/16x16', 'cr22' :'crystalsvg/22x22', 'cr32' :'crystalsvg/32x32',
			'cr48' :'crystalsvg/48x48', 'cr64' :'crystalsvg/64x64', 'cr128':'crystalsvg/128x128',
			'crsc' :'crystalsvg/scalable'
			}

		iconfiles = []
		dir=SCons.Node.FS.default_fs.Dir(path).srcnode()
		mydir=SCons.Node.FS.default_fs.Dir('.')
		import glob
		for ext in ['png', 'xpm', 'mng', 'svg', 'svgz']:
			files = glob.glob(str(dir)+'/'+'*-*-%s.%s' % (icname, ext))
			for file in files:
				iconfiles.append( file.replace(mydir.abspath, '') )
		for iconfile in iconfiles:
			lst = iconfile.split('/')
			filename = lst[ len(lst) - 1 ]
			tmp = filename.split('-')
			if len(tmp)!=3:
				env.pprint('RED','WARNING: icon filename has unknown format: '+iconfile)
				continue
			[icon_dir, icon_type, icon_filename]=tmp
			try:
				basedir=getInstDirForResType(lenv, restype)
				destdir = '%s/%s/%s/%s/' % (basedir, subdir, dir_dic[icon_dir], type_dic[icon_type])
			except KeyError:
				env.pprint('RED','WARNING: unknown icon type: '+iconfile)
				continue
			lenv.bksys_install(destdir, iconfile, icon_filename)

	header_ext = [".h", ".hxx", ".hpp", ".hh"]

	def KDEfiles(lenv, target, source):
		""" Returns a list of files for scons (handles kde tricks like .skel) 
		It also makes custom checks against double includes like : ['file.ui', 'file.cpp']
		(file.cpp is already included because of file.ui) """

		src=[]
		kcfg_files=[]
		other_files=[]
		kidl=[]

		source_=lenv.make_list(source)

		# For each file, check wether it is a dcop file or not, and create the complete list of sources
		for file in source_:

			sfile=SCons.Node.FS.default_fs.File(str(file))
			# why str(file) ? because ordinal not in range issues (scons bug)
			bs  = SCons.Util.splitext(file)[0]
			ext = SCons.Util.splitext(file)[1]
			if ext == '.skel':
				if not bs in kidl: kidl.append(bs)
				lenv.Dcop(bs+'.kidl')
				lenv.Depends(bs+'_skel.cpp', lenv['DCOPIDL2CPP'])
				src.append(bs+'_skel.cpp')
			elif ext == '.stub':
				if not bs in kidl: kidl.append(bs)
				lenv.Stub(bs+'.kidl')
				lenv.Depends(bs+'_stub.h', lenv['DCOPIDL2CPP'])
				lenv.Depends(bs+'_stub.cpp', lenv['DCOPIDL2CPP'])
				src.append(bs+'_stub.cpp')
			elif ext == '.moch':
				lenv.Moccpp(bs+'.h')
				src.append(bs+'_moc.cpp')
			elif ext == '.kcfgc': 
				name=SCons.Util.splitext(sfile.name)[0]
				hfile=lenv.Kcfg(file)
				cppkcfgfile=sfile.dir.File(bs+'.cpp')
				lenv.Depends(bs+'.cpp', lenv['KCONFIGCOMPILER'])
				src.append(bs+'.cpp')
			else:
				src.append(file)

		for base in kidl:lenv.Kidl(base+'.h')
		
		# Now check against typical newbie errors
		for file in kcfg_files:
			for ofile in other_files:
				if ofile == file:
					env.pprint('RED',"WARNING: You have included %s.kcfg and another file of the same prefix"%file)
					print "Files generated by kconfig_compiler (settings.h, settings.cpp) must not be included"
		return src

	#valid_targets = "program convenience shlib kioslave staticlib".split()
	from SCons.Script.SConscript import SConsEnvironment
	class kdeobj(SConsEnvironment.qt4obj):
		def __init__(self, val, senv=None):
			if senv: SConsEnvironment.qt4obj.__init__(self, val, senv)
			else: SConsEnvironment.qt4obj.__init__(self, val, env)
		def execute(self):
			if self.executed: return
			if self.orenv.has_key('DUMPCONFIG'):
				self.executed=1
				self.xml()
				return
			if (self.type=='shlib' or self.type=='kioslave'):
				self.instdir=self.orenv.getInstDirForResType('KDELIB')
			if self.type=='module':
				self.instdir=self.orenv.getInstDirForResType('KDEMODULE')
			elif self.type=='program':
				self.instdir=self.orenv.getInstDirForResType('KDEBIN')
				self.perms=0755

			self.env=self.orenv.Copy()
			#self.env2=self.orenv.Copy()

			self.source = KDEfiles(self.env, self.joinpath(self.target), self.joinpath(self.source))
			SConsEnvironment.qt4obj.execute(self)

		def xml(self):
			dirprefix = reldir('.')
			if not dirprefix: dirprefix=self.dirprefix
			ret='<compile type="%s" dirprefix="%s" target="%s" cxxflags="%s" cflags="%s" includes="%s" linkflags="%s" libpaths="%s" libs="%s" vnum="%s" libprefix="%s">\n' % (self.type, dirprefix, self.target, self.cxxflags, self.cflags, self.includes, self.linkflags, self.libpaths, self.libs, self.vnum, self.libprefix)
			if self.source:
				for i in self.orenv.make_list(self.source): ret+='    <source file="%s"/>\n' % i
			ret+="</compile>\n"
			self.orenv.add_dump(ret)

	from SCons.Script.SConscript import SConsEnvironment
	SConsEnvironment.KDEicon = KDEicon
	SConsEnvironment.kdeobj = kdeobj

	class kdeinitobj(kdeobj):
		def __init__(self, senv=None):
			if not senv:
				senv = env
			SConsEnvironment.kdeobj.__init__(self, 'shlib', senv)
			self.binary = kdeobj('program', senv)
			self.binary.libprefix = ''
			self.kdeinitlib = kdeobj('shlib', senv)
			self.kdeinitlib.libprefix = ''

		def execute(self):
			if self.executed: return

			# 'dcopserver' is the real one
			self.binary.target   = self.target
			# 'libkdeinit_dcopserver'
			self.kdeinitlib.target = 'libkdeinit_' + self.target
			# 'dcopserver' (lib)
			
			self.kdeinitlib.libs     = self.libs
			self.kdeinitlib.libpaths = self.libpaths
			self.kdeinitlib.uselib   = self.uselib
			self.kdeinitlib.source   = self.source
			self.kdeinitlib.execute()
					
			self.binary.uselib       = self.uselib
			self.binary.libs         = [self.kdeinitlib.target + ".la"] + self.orenv.make_list(self.libs)
			#self.binary.libdirs      = "build/dcop"
			self.binary.libpaths     = self.libpaths
			env.Depends(self.binary.target, self.kdeinitlib.target + ".la")

			myname=None
			myext=None
			for source in self.kdeinitlib.source:
				sext=SCons.Util.splitext(source)
				if sext[0] == self.target or not myname:
					myname = sext[0]
					myext  = sext[1]
					
			def create_kdeinit_cpp(target, source, env):
				""" Creates the dummy kdemain file for the binary"""
				dest=open(target[0].path, 'w')
				dest.write('extern \"C\" int kdemain(int, char* []);\n')
				dest.write('int main( int argc, char* argv[] ) { return kdemain(argc, argv); }\n')
				dest.close()
			env['BUILDERS']['KdeinitCpp'] = env.Builder(action=env.Action(create_kdeinit_cpp),
														prefix='kdeinit_', suffix='.cpp',
														src_suffix=myext)
			env.KdeinitCpp(myname)
			self.binary.source = "./kdeinit_" + myname + '.cpp'
			self.binary.execute()

			def create_kdeinit_la_cpp(target, source, env):
				""" Creates the dummy kdemain file for the module"""
				dest=open(target[0].path, 'w')
				dest.write('#include <kdemacros.h>\n')
				dest.write('extern \"C\" int kdemain(int, char* []);\n')
				dest.write('extern \"C\" KDE_EXPORT int kdeinitmain( int argc, char* argv[] ) { return kdemain(argc, argv); }\n')
				dest.close()
			env['BUILDERS']['KdeinitLaCpp'] = env.Builder(action=env.Action(create_kdeinit_la_cpp),
														  prefix='kdeinit_', suffix='.la.cpp',
														  src_suffix=myext)
			env.KdeinitLaCpp(myname)
			self.source = 'kdeinit_' + self.target + '.la.cpp'
			# TODO scons clean?
						
			SConsEnvironment.kdeobj.execute(self)

			
			
	SConsEnvironment.kdeinitobj = kdeinitobj

	# (js) apply KDE***FLAGS
	if env.has_key('KDECCFLAGS'):
		env.AppendUnique( CCFLAGS = env['KDECCFLAGS'] )
	if env.has_key('KDECXXFLAGS'):
		env.AppendUnique( CPPFLAGS = env['KDECXXFLAGS'] )
		env.AppendUnique( CXXFLAGS = env['KDECXXFLAGS'] )
