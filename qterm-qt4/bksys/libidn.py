# Copyright Thomas Nagy 2005
# BSD license (see COPYING)

"""
Find and load the libidn necessary compilation and link flags
"""

def exists(env):
	return true

def generate(env):
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_LIBIDN')):
		from SCons.Tool import Tool
		import os
		pkgs = Tool('pkgconfig', ['./bksys'])
		pkgs.generate(env)

		have_idn  = env.pkgConfig_findPackage('LIBIDN', 'libidn', '0.5.0')

    		dest=open(env.join(env['_BUILDDIR_'], 'config-libidn.h'), 'w')
    		dest.write('/* libidn configuration */\n')
		if have_idn: dest.write('#define HAVE_IDNA_H 1\n');
		dest.close()
		env['_CONFIG_H_'].append('libidn')
