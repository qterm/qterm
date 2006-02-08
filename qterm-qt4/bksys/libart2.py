# Copyright Thomas Nagy 2005
# BSD license (see COPYING)
def exists(env):
	return true

def generate(env):

	def Check_libart2(context):
		import SCons.Util
		context.Message('Checking for libart2 ... ')
		ret = context.TryAction('libart2-config --version')[0]
		if ret: 
			ver = os.popen('libart2-config --version').read().strip().split('.')
			if (int(ver[0])*1000000 + int(ver[1])*1000 + int(ver[2]))<2003008: 
				ret = False;
			else:
				env['CXXFLAGS_LIBART'] = SCons.Util.CLVar( 
					os.popen('libart2-config --cflags').read().strip() );
				env['LINKFLAGS_LIBART'] = SCons.Util.CLVar( 
					os.popen('libart2-config --libs').read().strip() );
		env['CACHED_LIBART'] = ret;
		context.Result(ret)
		return ret

	from SCons.Options import Options
	import os

	optionFile = env['CACHEDIR'] + 'libart.cache.py'
	opts = Options(optionFile)
	opts.AddOptions(
		('CACHED_LIBART', 'Whether libart2 is available'),
		('CXXFLAGS_LIBART',''),
		('LINKFLAGS_LIBART',''),
		)
	opts.Update(env)
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_LIBART')):
		conf = env.Configure(custom_tests =	 { 'Check_libart2' : Check_libart2 } )

		if not conf.Check_libart2():
			print 'libart2 >= 2.3.8 not found (mandatory).'

		env = conf.Finish()
		opts.Save(optionFile, env)
