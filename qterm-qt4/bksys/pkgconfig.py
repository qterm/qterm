# Copyright Thomas Nagy 2005
# BSD license (see COPYING)
def exists(env):
	return true

def generate(env):
	import SCons.Util, os

	# This funtion detects pkg-config
	from SCons.Script.SConscript import SConsEnvironment
	def Check_pkg_config(context, version):

		from SCons.Options import Options
		
		optionFile = env['CACHEDIR'] + 'pkgconfig.cache.py'
		opts = Options(optionFile)
		opts.AddOptions(('CACHE_PKGCONFIG', 'whether pkg-config was found'))
		opts.Update(env)

		context.Message('Checking for pkg-config ... ')
			
		if not env.has_key('CACHE_PKGCONFIG'):
			pkg_config_command = 'pkg-config'
			if os.environ.has_key("PKG_CONFIG_PATH"):
				pkg_config_command = "PKG_CONFIG_PATH="+os.environ["PKG_CONFIG_PATH"]+" pkg-config "
			ret = context.TryAction(pkg_config_command+' --atleast-pkgconfig-version=%s' % version)[0]
			env['CACHE_PKGCONFIG'] = ret
			opts.Save(optionFile, env)
		else:
			ret = env['CACHE_PKGCONFIG']
			
		context.Result(ret)
		
		return ret

	# This function detects a package using pkg-config
	def Check_package(context, pkgname, module, version):
		context.Message('Checking for %s >= %s ... ' % (module, version))
		pkg_config_command = 'pkg-config'
		if os.environ.has_key("PKG_CONFIG_PATH"):
			pkg_config_command = "PKG_CONFIG_PATH="+os.environ["PKG_CONFIG_PATH"]+" pkg-config "
		ret = context.TryAction(pkg_config_command+' %s --atleast-version=%s' % (module, version))[0]
		if ret:
			env.ParseConfig(pkg_config_command+' %s --cflags --libs' % module);
			env['CXXFLAGS_'+pkgname] = SCons.Util.CLVar( 
				os.popen(pkg_config_command+" %s --cflags 2>/dev/null" % module).read().strip() );
			env['LINKFLAGS_'+pkgname] = SCons.Util.CLVar( 
					os.popen(pkg_config_command+" %s --libs 2>/dev/null" % module).read().strip() );
		context.Result(ret)
		return ret

	def pkgConfig_findPackage(env, pkgname, module, version):
		if env['WINDOWS']:
			return 0 #pkgConfig_findPackage() doesn't work on win32, why? (js)
		from SCons.Options import Options

		optionFile = env['CACHEDIR'] + module + '.cache.py'
		opts = Options(optionFile)
		opts.AddOptions(
				('CACHED_'+pkgname, 'whether '+pkgname+' was found'),
				('CXXFLAGS_'+pkgname, 'additional compilation flags'),
				('LINKFLAGS_'+pkgname, 'link flags')
				)
		opts.Update(env)

		if not env.has_key('CACHED_'+pkgname):
			conf = env.Configure(custom_tests =
					     { 'Check_pkg_config' : Check_pkg_config,
					       'Check_package' : Check_package }
					     )

			if env.has_key('CXXFLAGS_'+pkgname):  env.__delitem__('CXXFLAGS_'+pkgname)
			if env.has_key('LINKFLAGS_'+pkgname): env.__delitem__('LINKFLAGS_'+pkgname)

			if not env.has_key('CACHE_PKGCONFIG'):
				if not conf.Check_pkg_config('0.15'):
					print 'pkg-config >= 0.15 not found.'
					env.Exit(1)

			haveModule = conf.Check_package(pkgname, module, version)
			env = conf.Finish()

			env['CACHED_'+pkgname] = haveModule

			opts.Save(optionFile, env)
		else:
			haveModule = env['CACHED_'+pkgname]
			
		return haveModule

	SConsEnvironment.pkgConfig_findPackage = pkgConfig_findPackage
