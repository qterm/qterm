# Copyright Thomas Nagy 2005
# BSD license (see COPYING)
def exists(env):
	return true

def generate(env):

	def Check_libpcre(context):
		import SCons.Util
		context.Message('Checking for libpcre ... ')
		ret = context.TryAction('pcre-config --version')[0]
		if ret: 
			env['CACHED_PCRE'] = 1;
			env['CXXFLAGS_PCRE'] = SCons.Util.CLVar( 
					os.popen('pcre-config --cflags').read().strip() )
			env['LINKFLAGS_PCRE'] = SCons.Util.CLVar( 
					os.popen('pcre-config --libs').read().strip() )
		else:
			env['CACHED_PCRE'] = 0;
		context.Result(ret)
		return ret

	from SCons.Options import Options
	import os

	optionFile = env['CACHEDIR'] + 'libpcre.cache.py'
	opts = Options(optionFile)
	opts.AddOptions(
		('CACHED_PCRE', 'Whether libpcre is available'),
		('CXXFLAGS_PCRE',''),
		('LINKFLAGS_PCRE',''),
		)
	opts.Update(env)
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_PCRE')):
		conf = env.Configure(custom_tests =	 { 'Check_libpcre' : Check_libpcre } )

		if not conf.Check_libpcre():
			print 'libpcre not found .'

		env = conf.Finish()
		opts.Save(optionFile, env)

