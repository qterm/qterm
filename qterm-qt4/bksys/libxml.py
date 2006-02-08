# Copyright Thomas Nagy 2005
# BSD license (see COPYING)

"""
Find and load the libxml2 and libxslt necessary compilation and link flags
"""

def exists(env):
	return true

def generate(env):
	if env['WINDOWS']:
		return #generate() doesn't work on win32, why? (js)
	if not env['HELP'] and (env['_CONFIGURE_'] or not env.has_key('CACHED_XML')):
		from SCons.Tool import Tool
		pkgs = Tool('pkgconfig', ['./bksys'])
		pkgs.generate(env)

		have_xml  = env.pkgConfig_findPackage('XML', 'libxml-2.0', '2.6.0')
		have_xslt = env.pkgConfig_findPackage('XSLT', 'libxslt', '1.1.0')

		# if the config worked, read the necessary variables and cache them
		if not have_xml:
			env.pprint('RED', 'libxml-2.0 >= 2.6.0 was not found (mandatory).')
			env.Exit(1)
		
		if not have_xslt:
			env.pprint('RED', 'libxslt-2.0 >= 1.1.0 was not found (mandatory).')
			env.Exit(1)

