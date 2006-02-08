#! /usr/bin/env python

## 
# @file 
# bksys Makefile.am related tools
# 
# note: may be a future base of a Makefile.am to scons converter if someone like to write it 
#
# (@) 2005 Ralf Habacker  - published under the GPL license
#
 
import os, re, types, sys, string, shutil, stat, glob

## print a dictionary 
def printDict(dict,sep=' = ' ):
	for key in dict.keys():
		print key  + sep + dict[key]


## Makefile.am support class
#
# The class provides methods for the following tasks: 
#		- parsing Makefile.am's and collecting targets and defines into class members 
#		- extracting library dependencies and linker flags keyed by the related targets
#		- extracting target libraries and programs
#		- collecting detailled lists of libraries dependencies
#
#
class AMFile: 
	def __init__(self):
		self.defines = {}
		self.targets = {}
		self.libadds = {}
		self.ldflags = {}
		self.libs    = {}
		self.progs   = {}
		self.sources = {}
		self.includes= {}
		self.headers = {}
		self.data    = {}
		self.datadirs= {}
		self.path    = ''
		self.subdirs = ''
		
	## read and parse a Makefile.am 
	#
	# The resulting lines are stored in the defines and targets class member.
	# note: Multiple lines in a target are separated by '###'
	# @param path - path for Makefile.am
	# @return 0 if file couldn't be read 
	# 
	def read(self,path):
		try:
			src=open(path, 'r')
		except:
			return 0
		self.path = path
		
		file  = src.read()
		lines = file.replace('\n\t','###')
		list = lines.replace('\\\n',' ').split('\n')
		for line in list:
			if line[:1] == '#' or len(line) == 0:
				continue
			
			index = 0
			while line[index].count('#'):
				index = line[index].index('#')
				if line[index:index+3] == "###":
					index += 3

			var = line.split('=')
			if len(var) == 2:
				self.defines[str(var[0]).strip()] = var[1].strip().replace("'",'').replace('\###',' ')
			else:
				target = line.split(':')
				if len(target) == 2:
					single_target = target[1].strip().replace("'",'')
					# TODO: (rh) split into list 
					self.targets[str(target[0]).strip()] = single_target
		self.getLibraries()
		self.getPrograms()
		self.getIncludes()
		self.getHeaders()
		self.getSources()
		self.getLibraryDeps()
		self.getLinkerFlags()
		self.getSubdirs()
		self.getData()

		return 1
		
	## adds library dependencies from another AMFile instance 
	#
	# This method is mainly used for an instance collecting definitions 
	# from instances from lower levels 
	# @param src - AMFile instance, from which the dependencies are imported 
	#
	def addLibraryDeps(self,src):
		for key in src.libadds.keys():
			self.libadds[key] = src.libadds[key]
				
	## adds linker flags from another AMFile instance 
	#
	# This method is mainly used for an instance collecting definitions 
	# from instances from lower levels 
	# @param src - AMFile instance, from which the flags are imported 
	#
	def addLinkerFlags(self,src):
		for key in src.ldflags.keys():
			self.ldflags[key] = src.ldflags[key]

	def addSources(self,src):
		for key in src.sources.keys():
			self.sources[key] = src.sources[key]

	def addLibraries(self,src):
		for key in src.libs.keys():
			self.libs[key] = src.libs[key]

	def getSubdirs(self):
		if self.defines.has_key('SUBDIRS'):
			self.subdirs = self.defines['SUBDIRS']
			del self.defines['SUBDIRS']
	## collect all LIBADDS definitions 
	#
	# the function store the definitions in the libadds class member keyed 
	# by the relating target
	# @return definition list 
	#
	def getLibraryDeps(self):
		reg = re.compile("(.*?)_(?:l?a_)?LIBADD$")
		# TODO (rh) fix relative library pathes 
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				libadd = self.findRealTargetname(str(result.group(1)))
				if not len(libadd):
					print "WARNING: no corresponding target for libadd item %s \n" % str(result.group(1))
					continue
				self.libadds[libadd] = self.defines[key]
				del self.defines[key]
		return self.libadds

	## collect all LDFLAGS definitions 
	#
	# the function store the definitions in the ldflags class member keyed 
	# by the relating target 
	# @return definition list 
	#
	def getLinkerFlags(self):
		reg = re.compile("(.*?)_(?:l?a_)?LDFLAGS$")
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				ldflag = self.findRealTargetname(str(result.group(1)))
				if not len(ldflag):
					print "WARNING: no corresponding target for ldflag item %s \n" % str(result.group(1))
					continue
				self.ldflags[ldflag] = self.defines[key]
				del self.defines[key]
		return self.ldflags

	## collect all LTLIBRARIES definitions 
	#
	# the function store the definitions in the libraries class member keyed 
	# by the relating target 
	# @return definition list 
	#
	def getLibraries(self):
		def stripLibname(val):
			reg = re.compile("(?:lib)?(.*)\.l?a$")
			files = val.split()
			retlist = []
			for file in files:
				result=reg.match(file)
				if result:
					retlist.append(str(result.group(1)))
			return ' '.join(retlist)
		reg = re.compile("(.*)(_l?a)?_LTLIBRARIES$")
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				libtype = str(result.group(1))
				if not self.libs.has_key(libtype):
					self.libs[libtype] = ""
				self.libs[libtype] = stripLibname(self.defines[key])
				del self.defines[key]
		return self.libs

	def getPrograms(self):
		reg = re.compile("(.*)_PROGRAMS$")
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				progtype = str(result.group(1))
				if not self.progs.has_key(progtype):
					self.progs[progtype] = ""
				self.progs[progtype] = self.defines[key]
				del self.defines[key]
		return self.progs

	def getData(self):
		reg = re.compile("(.*)_DATA$")
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				dat = str(result.group(1))
				self.data[dat] = self.defines[key]
				del self.defines[key]
		for name in self.data.keys():
			if self.defines.has_key(name+"dir"):
				self.datadirs[name] = self.defines[name+"dir"]
				del self.defines[name+"dir"]
			else:
				del self.data[name]
		
		return self.data

	## collect all SOURCES definitions 
	#
	# the function store the definitions in the sources class member keyed 
	# by the relating target
	# this function should be called after getPrograms and getLibraries
	# @return definition list 
	#
	def getSources(self):
		reg = re.compile("(.*?)METASOURCES$")
		reg = re.compile("(.*?)_(?:l?a_)?SOURCES$")
		# TODO (rh) fix relative library pathes 
		for key in self.defines.keys():
			if key.endswith('METASOURCES') and self.defines[key] == "AUTO":
				del self.defines[key]
				continue
			
			result=reg.match(key)
			if result:
				source = self.findRealTargetname(str(result.group(1)))
				if not len(source):
					print "WARNING: no corresponding target for source item %s \n" % str(result.group(1))
					continue
				self.sources[source] = self.defines[key]
				del self.defines[key]
		return self.sources

	def findRealTargetname(self, target):
		def findInDict(dict, target):
			for key in dict.keys():
				targets = dict[key].split()
				if targets.count(target) > 0:
					return 1
			return 0
		if not findInDict(self.progs, target) and not findInDict(self.libs, target):
			if target[:3] == "lib":
				target = target[3:]
				if not findInDict(self.libs, target):
					target = ""
			else:
				target = ""
		return target
	
	def getIncludes(self):
		#if we've got a dir global includes, save it in self.includes['_DIR_GLOBAL_']
		if self.defines.has_key('INCLUDES'):
			self.defines['_DIR_GLOBAL__INCLUDES'] = self.defines['INCLUDES']
			del self.defines['INCLUDES']
		reg = re.compile("(.*?)_(?:l?a_)?INCLUDES")
		# TODO (rh) fix relative library pathes 
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				include = self.findRealTargetname(str(result.group(1)))
				if not len(include):
					include = result.group(1)
				self.includes[include] = self.defines[key]
				del self.defines[key]
		return self.includes

	def convertMakeFileVariable(self, var):
		var = var.strip()
		if var[:2] == '$(' and var[-1:] == ')':
			return var[2:-1]
		else:
			return var

	def getHeaders(self):
		#if we've got a dir global includes, save it in self.headers['_DIR_GLOBAL_']
		if self.defines.has_key('HEADERS'):
			self.defines['_DIR_GLOBAL__HEADERS'] = self.defines['HEADERS']
			del self.defines['HEADERS']
		reg = re.compile("(.*?)_(?:l?a_)?HEADERS")
		# TODO (rh) fix relative library pathes 
		for key in self.defines.keys():
			result=reg.match(key)
			if result:
				header = self.findRealTargetname(str(result.group(1)))
				if not len(header):
					header = result.group(1)
				self.headers[header] = self.defines[key]
				del self.defines[key]
		return self.headers
			
	## return a reverse usage list of dependencies 
	#
	# The function scannes the recent library definitions and reorganice
	# the resulting list keyed by the used library
	# @return dependency list 
	#
	def getReverseLibraryDeps(self):
		alist = {}
		for key in self.libadds.keys():
			for lib in self.libadds[key].split():
				if lib in alist:
					alist[str(lib)] += " " + key
				else:
					alist[str(lib)] = key
		return alist
	
	def printDefines(self):
		print "### DEFINES:" 
		printDict(self.defines,' = ')
	
	def printTargets(self):
		print "### TARGETS:" 
		printDict(self.targets,' : ')

	def printLibraryDeps(self):
		print "### LIBADD:" 
		printDict(self.libadds,' : ')

	def printLinkerFlags(self):
		print "### LDFLAGS:"
		printDict(self.ldflags,' : ')

	def printLibraries(self):
		print "### Libraries:"
		printDict(self.libs,' : ')

	def printSources(self):
		print "### Sources:"
		printDict(self.sources,' : ')

# uses = 0
# libadds = 0
# ldflags = 0
# defines = 0
# targets = 0
# libs = 0
# sources = 0
# if len(sys.argv) == 1:
# 	print "amtool [options] Makefile.am [Makefile.am] ..."
# 	print "list Makefile.am content" 
# 	print "options:" 
# 	print "    --uses print where a library is used" 
# 	print "    --libadd print all LIBADD depenencies " 
# 	print "    --ldflags print all LDFLAGS definitions" 
# 	print "    --defines print all Makefile variables" 
# 	print "    --targets print all Makefile tarets" 
# 	print "    --libs print all libraries defined in self Makefile.am" 
# 	print "    --sources print all sources defined in self Makefile.am" 
# else:
# 	all_ams = AMFile()
# 	for a in range(1,len(sys.argv)):
# 		if sys.argv[a][:6] == '--uses':
# 			uses = 1
# 		elif sys.argv[a][:8] == '--libadd':
# 			libadds = 1
# 		elif sys.argv[a][:9] == '--defines':
# 			defines = 1
# 		elif sys.argv[a][:9] == '--targets':
# 			targets = 1			
# 		elif sys.argv[a][:9] == '--ldflags':
# 			ldflags = 1
# 		elif sys.argv[a][:6] == '--libs':
# 			libs = 1
# 		elif sys.argv[a][:9] == '--sources':
# 			sources = 1
# 		if  libadds or defines or targets or ldflags or libs or sources:	
# 			uses = 2
# 						
# 	for a in range(1,len(sys.argv)):
# 		if sys.argv[a][:2] == '--':
# 			continue
# 		am_file = AMFile()
# 
# 		if not am_file.read(sys.argv[a]): 
# 			continue
# 
# 		if uses == 2:
# 			print "### " + sys.argv[a]
# 
# 		if defines:
# 			am_file.printDefines()
# 		if targets:
# 			am_file.printTargets()
# 		if libadds:
# 			am_file.printLibraryDeps()
# 		if ldflags:
# 			am_file.printLinkerFlags()
# 		if libs:
# 			am_file.printLibraries()
# 		if sources:
# 			am_file.printSources()
# 
# 		all_ams.addLibraryDeps(am_file)
# 		all_ams.addLinkerFlags(am_file)
# 		all_ams.addSources(am_file)
# 		all_ams.addLibraries(am_file)
# 		generateConscript(am_file, out_buf)
		
# 	if uses == 0:
# 		all_ams.printLibraryDeps()
# 	elif uses == 1:
# 		a = all_ams.getReverseLibraryDeps()
# 		printDict(a)
