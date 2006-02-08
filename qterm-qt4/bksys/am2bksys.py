#! /usr/bin/env python
## 
# @file 
# bksys Makefile.am related tools
# 
# a Makefile.am to scons converter
#
# (@) 2005 LiuCougar  - published under the GPL license
#

import os, re, types, sys, string, shutil, stat, glob, amtool

## Class for automatically converting Makefile.am to SConscript
#
# The class can handle simple Makefile.am 100% correct, for complex ones, manual
# fine-tunning is required. These are something that can not handle:
#		- Custom make target
#		- extra moc files
#		- checking programs (test/EXTRA, how to enable checking?? )
#		- conditional compilation
#		- install dir support
#		- DATA files handling
#
#

class AM2Bksys(amtool.AMFile):
	def __init__(self):
		amtool.AMFile.__init__(self)
		self.ignoreIncludePaths = ['$(top_srcdir)', '$(all_includes)']

		self.defaultUseLibs = 'QT QTCORE QTGUI QT3SUPPORT KDE4'

		self.LibHandlerMapping = {
		'noinst' : 'env.kdeobj(\'convenience\')',
		'kdeinit' : 'env.kdeinitobj()',
		'lib' : 'env.kdeobj(\'shlib\')\nobj.it_is_a_kdelib()',
		'kde_module' : 'env.kdeobj(\'module\')',
		}
		
		self.ProgHandlerMapping = {
		'bin' : 'env.kdeobj(\'program\')',
		'check' : 'env.kdeobj(\'program\')\nobj.env = env.Copy()\nobj.env[\'NOAUTOINSTALL\'] = 1',
		'EXTRA' : 'env.kdeobj(\'program\')\nobj.env = env.Copy()\nobj.env[\'NOAUTOINSTALL\'] = 1',
		}
		
		self.DataHandlerMapping = {
			'kde_htmldir' : '',
			'kde_appsdir' : 'KDEAPPS',
			'kde_icondir' : 'KDEICONS',
			'kde_sounddir' : '',
			'kde_datadir' : 'KDEDATA',
			'kde_locale' : 'KDELOCALE',
			'kde_cgidir' : '',
			'kde_confdir' : 'KDECONF',
			'kde_kcfgdir' : 'KDEKCFG',
			'kde_mimedir' : 'KDEMIME',
			'kde_toolbardir' : '',
			'kde_wallpaperdir' : '',
			'kde_templatesdir' : '',
			'kde_bindir' : 'KDEBIN',
			'kde_servicesdir' : 'KDESERV',
			'kde_servicetypesdir' : 'KDESERVTYPES',
			'kde_moduledir' : 'KDEMODULE',
			'kde_styledir' : '',
			'kde_widgetdir' : '',
			'xdg_appsdir' : '',
			'xdg_menudir' : '',
			'xdg_directorydir' : '',
		}
		
		self.out_buf_header = """#! /usr/bin/env python
#generated from %s by am2bksys.py
Import('env')

"""
	#generate source lists
	def generateSources(self):
		out_buf = ""
		for key in self.sources.keys():
			out_buf += key + "_sources = \"\"\"\n"
			sources = self.sources[key].split()
			sources.sort()
			
			while len(sources) >= 4:
				out_buf += ' '.join(sources[:4]) + "\n"
				del sources[:4]
			if len(sources):
				out_buf += ' '.join(sources) + "\n"
			out_buf += "\"\"\"\n\n"
		return out_buf

	#generate header file lists
	def generateHeaders(self):
		out_buf = ""
		for key in self.headers.keys():
			out_buf += key + "_headers = \"\"\"\n"
			headers = self.headers[key].split()
			headers.sort()
			
			while len(headers) >= 4:
				out_buf += ' '.join(headers[:4]) + "\n"
				del headers[:4]
			if len(headers):
				out_buf += ' '.join(headers) + "\n"
			out_buf += "\"\"\"\n\n"
		return out_buf

	def generateLibadds(self, name):
		reg = re.compile("(.*)lib([^/]*)\.la$")
		def convertLocalLibs(lib):
			lib = lib.replace('$(top_srcdir)', '#')
			lib = lib.replace('$(top_builddir)', '##')
			lib = lib.replace('$(srcdir)/', '')
			result=reg.match(lib)
			if result:
				path = result.group(1)
				if path.endswith('/'):	path = path[:-1]
				if path[:2] == "./": path = path[2:]
				return (path, result.group(2))
			
			return ('', lib)
		uselibs = self.defaultUseLibs.split()
		out_buf = ""
		if self.libadds.has_key(name):
			convertedlibs = []
			libpaths = []
			libs = self.libadds[name].split()
			for lib in libs:
				if lib[:2] == '$(' and lib[-1:] == ')':
					if lib[2:6] == "LIB_":
						uselibs.append(lib[6:-1])
					elif lib[2:5] == "LIB":
						uselibs.append(lib[5:-1])
					else:
						print "WARNING: UNKNOW makefile variable in LDADDS %s" % lib
				else:
					path, libname = convertLocalLibs(lib)
					if len(path) and not libpaths.count(path):
						libpaths.append(path)
					convertedlibs.append(libname)
			if len(convertedlibs):
				out_buf += "obj.libpaths = '%s '\n" % ' '.join(libpaths)
				out_buf += "obj.libs = '%s '\n" % ' '.join(convertedlibs)
		
		out_buf += 'obj.uselib = \'%s \'\n' % ' '.join(uselibs)
		return out_buf

	def generateObj(self, islib, objtype, name):
		if islib:
			out_buf = "obj = %s\n" % self.LibHandlerMapping[objtype]
		else:
			out_buf = "obj = %s\n" % self.ProgHandlerMapping[objtype]

		include = ""
		
		#TARGET
		out_buf += "obj.target = '%s'\n" % name
		
		#FIXME: other LDFLAGS support
		#Version number
		if self.ldflags.has_key(name):
			flags = self.ldflags[name].split()
			if islib and flags.count('-version-info') > 0:
				index = flags.index('-version-info') + 1
				if index < len(flags):
					version = flags[index]
					if version.replace(':','').isdigit():
						out_buf += "obj.vnum = '%s'\n" % version.replace(':','.')
					else:
						print "WARNING: version-info format is incorrect"
		
		#SOURCES
		out_buf += "obj.source = %s_sources\n" % name
		
		#INCLUDES
		if self.includes.has_key(name):
			include = "'" + self.includes[name] + "'"
		elif self.includes.has_key('_DIR_GLOBAL_'):
			include = 'includes'
		
		if include:
			out_buf += "obj.includes = %s\n" % include
		
		#LIBADD
		out_buf += self.generateLibadds(name)
		
		out_buf += 'obj.execute()\n\n'
		
		return out_buf

	def generateSubdirs(self):
		out_buf = ""
		if len(self.subdirs):
			dirs = self.subdirs.split()
			if dirs.count('.'): del dirs[dirs.index('.')]
			out_buf = "env.subdirs('%s')\n\n" % " ".join(dirs)
		
		return out_buf;

	def generateData(self, files, inst_dir):
		def getKDEInstType(rawdir):
			if rawdir[:2] != "$(":
				print "ERROR: Do not understand data install dir format %s\n" % rawdir
				return "",""
			instype = subdir = ""
			if rawdir.endswith(")"):
				instype = rawdir[2:-1]
			else:
				reg = re.compile("\$\((.*)\)/(.*)")
				result = reg.match(rawdir)
				if result:
					instype = result.group(1)
					subdir = result.group(2)
					
			if not len(instype) or not self.DataHandlerMapping.has_key(instype):
				instype = ""
			else:
				instype = self.DataHandlerMapping[instype]
				
			return (instype, subdir)
			
		out_buf = ""
		if len(files):
			(insttype, subdir) = getKDEInstType(inst_dir)
			if insttype:
				out_buf = "env.bksys_insttype('%s', '%s', '%s')\n\n" % (insttype, subdir, files)
		return out_buf

	def generateConscript(self):
		out_buf = self.out_buf_header % self.path
		
		self.getIncludes()	#fix path
		
		out_buf += self.generateSubdirs()
		
		out_buf += self.generateSources()

		out_buf += self.generateHeaders()

		#find global includes setting for self dir:
		if self.includes.has_key('_DIR_GLOBAL_'):
			out_buf += "includes = '%s '\n\n" % self.includes['_DIR_GLOBAL_']
			
		#generate library objects
		for libtypekey in self.libs.keys():
			if not self.LibHandlerMapping.has_key(libtypekey):
				print "ERROR: library type %s is not defined in LibHandlerMapping" % libtypekey
				return 
			for key in self.libs[libtypekey].split():
				out_buf += self.generateObj(1, libtypekey, key)
			
		#generate program objects
		for progtypekey in self.progs.keys():
			if not self.ProgHandlerMapping.has_key(progtypekey):
				print "ERROR: Program type %s is not defined in ProgHandlerMapping" % progtypekey
				return
			#FIXME: implement test/check support
			if progtypekey in ['check', 'EXTRA']:
				continue
			for key in self.progs[progtypekey].split():
				out_buf += self.generateObj(0, progtypekey, key)
		
		#generate data files
		for dataname in self.data.keys():
			out_buf += self.generateData(self.data[dataname], self.datadirs[dataname])
		
		if len(self.defines):
			out_buf += '\n"""Unhandled Defines \n'
			for key in self.defines.keys():
				out_buf += key  + ' = ' + self.defines[key] + "\n"
			out_buf += '"""\n'
		
		if len(self.targets):
			out_buf += '\n"""Unhandled Targets \n'
			for key in self.targets.keys():
				out_buf += key  + ' = ' + self.targets[key] + "\n"
			out_buf += '"""\n'
		
		return out_buf
		
	def getIncludes(self):
		amtool.AMFile.getIncludes(self)
		for key in self.includes.keys():
			self.includes[key] = self.convertIncludesPath(self.includes[key])
		return self.includes

	def convertIncludesPath(self, paths_str):
		retlist = []
		paths = paths_str.split()
		for path in paths:
			if path[:2] == '-I':
				path = path[2:]
			if path in self.ignoreIncludePaths:
				continue
			path = path.replace('$(top_srcdir)', '#')
			path = path.replace('$(top_builddir)', '#')
			path = path.replace('$(srcdir)/', '')
			retlist.append(self.convertMakeFileVariable(path))
		return ' '.join(retlist)
	
if len(sys.argv) == 1:
	print "am2bksys [options] Makefile.am"
	print "Convert a Makefile.am to bksys SConscript"
	print "options:" 
	print "    -o file  Write generated SConscript to this file" 
	print "    -w Write generated SConscript to the same dir as the Makefile.am" 
	print "By default, am2bksys will output the generated SConscript to stdout" 
else:
	outputfile = ""
	for a in range(1,len(sys.argv)):
		if sys.argv[a][:2] == '-o' and a+1 < len(sys.argv):
			outputfile = sys.argv[a+1]
			a += 1
		elif sys.argv[a][:2] == '-w':
			outputfile = "#"

	for a in range(1,len(sys.argv)):
		if sys.argv[a][:1] == '-':
			continue
		
		am_file = AM2Bksys()
		inputfile = sys.argv[a];
		if not am_file.read(inputfile):
			print "ERROR: can not read file %s" % inputfile
			continue
		
		if len(outputfile):
			try:
				if outputfile == "#":
					outputfile = inputfile.replace("Makefile.am", "SConscript")
				dest = open(outputfile, 'w')
			except:
				exit
			
			dest.write(am_file.generateConscript())
			dest.close()
			print "File %s is successfully created." % outputfile
		else:
			print am_file.generateConscript()
		
		break