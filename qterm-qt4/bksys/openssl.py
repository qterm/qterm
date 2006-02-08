# Copyright liucougar 2005
# BSD license (see COPYING)

"""
Find and load the openssl necessary compilation and link flags
"""

def exists(env):
	return true

def generate(env):
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_OPENSSL')):
		from SCons.Tool import Tool
		import os
		pkgs = Tool('pkgconfig', ['./bksys'])
		pkgs.generate(env)

		# TODO this will not work on opensuse 10 (ita) openssl-devel not installed
		have_openssl = env.pkgConfig_findPackage('OPENSSL', 'openssl', '0.9.6')

		dest=open(env.join(env['_BUILDDIR_'], 'ksslconfig.h'), 'w')
		dest.write('/* openssl configuration */\n')
		if have_openssl: dest.write('#define KSSL_HAVE_SSL 1\n');
		dest.close()
		#env['_CONFIG_H_'].append('openssl')
