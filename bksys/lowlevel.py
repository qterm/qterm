# Copyright Thomas Nagy 2005
# BSD license (see COPYING)

"""
This tool is used to find and load the libxml2 and libxslt
neceessary compilation and link flags
"""

def exists(env):
	return true

## TODO documentation
##############  -- IMPORTANT --
## warning: the naming matters, see genobj -> uselib in generic.py
## warning: parameters are lists
## obj.uselib='Z PNG KDE4' will add all the flags
##   and paths from zlib, libpng and kde4 vars defined below


#
# Check for a function (e.g. usleep or strlcat) for which we have a replacement available
# in kdecore/fakes.c (libkdefakes).
# (This is a replacement for KDE_CHECK_FUNC_EXT)
def CheckFuncWithKdefakeImpl(context, dest, function_name, header, sample_use, prototype):
	code="""
%(header)s
int main() {
	%(sample_use)s;
	return 0;
}
	""" % { 'header' : header, 'sample_use' : sample_use }
	context.Message("Checking for function %s()... " % (function_name))
	ret = context.TryLink(code, '.cpp')
	if ret:
		import string
		dest.write("#define HAVE_%s 1\n" % (string.upper(function_name)))
		context.Result(ret)
	else:
                context.Result('ok - in libkdefakes')
                dest.write("""
/* No %(function_name)s function found on the system, kdefakes will provide it. */
#ifdef __cplusplus
extern "C" {
#endif
%(prototype)s;
#ifdef __cplusplus
}
#endif
""" % { 'prototype' : prototype, 'function_name' : function_name } )
	return ret


## TODO move to platform lowlevel.py
## DF: only the platform-dependent parts, right?
def generate(env):
	if env['HELP']: return

	import sys

	#######################################
	## other stuff

	if env['WINDOWS']:
		env['CONVENIENCE']         = []
	else:
		if sys.platform == 'darwin':
			env['CONVENIENCE']         = ['-fPIC','-DPIC','-fno-common'] # TODO flags for convenience libraries
		else:
			env['CONVENIENCE']         = ['-fPIC','-DPIC'] # TODO flags for convenience libraries

	########## zlib
	if env['WINDOWS'] and env['CC'] == 'cl':
		env['LIB_Z']         = ['zlib']
	else:
		env['LIB_Z']         = ['z']

	########## png
	env['LIB_PNG']             = ['png', 'm'] + env['LIB_Z']

	if not env['WINDOWS']:
		########## sm
		env['LIB_SM']              = ['SM', 'ICE']
	
		env['LIB_DL']              = ['dl']

		########## X11
		env['LIB_X11']             = ['X11']
		env['LIBPATH_X11']         = ['/usr/X11R6/lib/']
		env['LIB_XRENDER']         = ['Xrender']

	# tell it we do have a global config.h file
	env['_CONFIG_H_'].append('lowlevel')

	from SCons.Options import Options
	cachefile=env['CACHEDIR']+'lowlevel.cache.py'
	opts = Options(cachefile)
	opts.AddOptions(
	        ('LOWLEVEL_ISCONFIGURED', ''),
	)
	opts.Update(env)

	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('LOWLEVEL_ISCONFIGURED')):
		env['_CONFIGURE_']=1
		import sys
		import os

		## Didn't generic.py do that already?
		if not os.path.exists( env['_BUILDDIR_'] ): os.mkdir( env['_BUILDDIR_'] )
		
		## The platform-independent checks
		dest=open(env.join(env['_BUILDDIR_'], 'config-lowlevel.h'), 'w')
		dest.write('/* lowlevel configuration */\n')

		conf = env.Configure( custom_tests = { 'CheckFuncWithKdefakeImpl' : CheckFuncWithKdefakeImpl } )
		## TODO let the caller specify which checks they want? Hopefully not turning this into one-file-per-check though...
        	conf.CheckFuncWithKdefakeImpl(dest, 'setenv', '#include <stdlib.h>',
					      'setenv("VAR", "VALUE", 1);',
					      'int setenv (const char *, const char *, int)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'unsetenv', '#include <stdlib.h>',
					      'unsetenv("VAR");',
					      'void unsetenv (const char *)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'getdomainname', '#include <stdlib.h>\n#include <unistd.h>\n#include <netdb.h>',
					      'char buffer[200]; getdomainname(buffer, 200);',
					      '#include <sys/types.h>\nint getdomainname (char *, size_t)')
        	# TODO: does not work yet on mingw
        	if not (env['WINDOWS'] and env['CC'] == 'gcc'):
        	        conf.CheckFuncWithKdefakeImpl(dest, 'gethostname', '#include <stdlib.h>\n#include <unistd.h>',
					      'char buffer[200]; gethostname(buffer, 200);',
					      'int gethostname (char *, unsigned int)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'usleep', '#include <unistd.h>',
					      'sleep (200);',
					      'int usleep (unsigned int)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'random', '#include <stdlib.h>',
					      'random();',
					      'long int random(void)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'srandom', '#include <stdlib.h>',
					      'srandom(27);',
					      'void srandom(unsigned int)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'initgroups', '#include <sys/types.h>\n#include <unistd.h>\n#include <grp.h>',
					      'char buffer[200]; initgroups(buffer, 27);',
					      'int initgroups(const char *, gid_t)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'mkstemps', '#include <stdlib.h>\n#include <unistd.h>',
					      'mkstemps("/tmp/aaaXXXXXX", 6);',
					      'int mkstemps(char *, int)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'mkstemp', '#include <stdlib.h>\n#include <unistd.h>',
					      'mkstemp("/tmp/aaaXXXXXX");',
					      'int mkstemp(char *)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'mkdtemp', '#include <stdlib.h>\n#include <unistd.h>',
					      'mkdtemp("/tmp/aaaXXXXXX");',
					      'char* mkdtemp(char *)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'strlcpy', '#include <string.h>',
					      'char buf[20]; strlcpy(buf, "KDE function test", sizeof(buf));',
					      'unsigned long strlcpy(char*, const char*, unsigned long)')
        	conf.CheckFuncWithKdefakeImpl(dest, 'strlcat', '#include <string.h>',
					      'char buf[20]; buf[0]=0; strlcat(buf, "KDE function test", sizeof(buf));',
					      'unsigned long strlcat(char*, const char*, unsigned long)')
		# TODO finish (AC_CHECK_RES_QUERY and AC_CHECK_DN_SKIPNAME)
		# TODO AC_CHECK_RES_INIT is a bit more complicated

		env = conf.Finish()

		## The platform-dependent checks

		if sys.platform == 'darwin':
			sys.path.insert(0,'bksys'+os.sep+'osx')
			from detect_lowlevel import detect
		elif env['WINDOWS']:
			sys.path.insert(0,'bksys'+os.sep+'win32')
			from detect_lowlevel import detect
		else:
			sys.path.insert(0,'bksys'+os.sep+'unix')
			from detect_lowlevel import detect
		detect(env, dest)
		dest.close()

		env['LOWLEVEL_ISCONFIGURED']=1
		opts.Save(cachefile, env)

