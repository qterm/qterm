# Copyright Thomas Nagy 2005
# BSD license (see COPYING)
def exists(env):
	return true

def generate(env):

	from SCons.Options import Options
	import os

	optionFile = env['CACHEDIR'] + 'fam.cache.py'
	opts = Options(optionFile)
	opts.AddOptions(
		('CACHED_FAM', 'Whether the FAM library is available'),
		('CXXFLAGS_FAM',''),
		('LINKFLAGS_FAM',''),
		)
	opts.Update(env)
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_FAM')):
		conf = env.Configure( )
		if conf.CheckCXXHeader('fam.h') and conf.CheckLib('fam','FAMOpen'):
			env['CXXFLAGS_FAM']= ['-DHAVE_FAM']
			env['LINKFLAGS_FAM'] = ['-lfam']
			env['CACHED_FAM'] = 1;
		else:
			env['CACHED_FAM'] = 0;
			print 'FAM not found.'

		env = conf.Finish()
		opts.Save(optionFile, env)
