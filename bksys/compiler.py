# Copyright David Faure <faure@kde.org> 2005
# BSD license (see COPYING)
def exists(env):
	return true

def generate(env):

	def get_compiler_version():
		import re, os

		# TODO check this with other compilers
		# TODO: and convert xlC and KCC code from kdelibs/configure.in.in
		if env['CC'] != 'gcc' and env['CC'] != 'cl':
			return False
	
		error_regexp = re.compile('Usage|ERROR|unrecognized option|unknown option|WARNING|missing|###')
		unused_regexp = re.compile('Reading specs|Using built|Configured with|Thread model|Copyright|###')
	
		for flag in ['-v', '-V', '--version', '-version', '/help']:
			# should I use context.TryAction here?
			syspf = os.popen( env['CC']+' '+flag+' 2>&1' )
			for line in syspf.read().split( '\n' ):
				if re.search( error_regexp, line, 'I' ):
					continue
				if re.search( unused_regexp, line, 'I' ):
					continue
				if len(line):
					return line

		return False

	def Check_compiler(context):
		context.Message('Checking for compiler version... ')
		import sys, os

		compiler_version = get_compiler_version()
		if compiler_version:
			# Quote backslashes, as we are going to make this a string
			compiler_version.replace('\\', '\\\\')

			env['_CONFIG_H_'].append('compiler')
			dest = open(env.join(env['_BUILDDIR_'], 'config-compiler.h'), 'w')
			dest.write('/* compiler name and version */\n')
			dest.write('#define KDE_COMPILER_VERSION "'+compiler_version+'"\n')
			dest.close();
			context.Result(compiler_version)
		else:
			print 'Compiler not found.'
			context.Result(False)
			env.Exit(1)

		
	from SCons.Options import Options

	optionFile = env['CACHEDIR'] + 'compiler.cache.py'
	opts = Options(optionFile)
	opts.AddOptions(
		('COMPILER_ISCONFIGURED', 'If the compiler is already tested for'),
		)
	opts.Update(env)
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('COMPILER_ISCONFIGURED')):
		conf = env.Configure(custom_tests =	 { 'Check_compiler' : Check_compiler } )

		conf.Check_compiler()

		env = conf.Finish()
		env['COMPILER_ISCONFIGURED'] = 1;
		opts.Save(optionFile, env)
