#####################################################################
#
# util/autorun.py
#
# Imports all modules from a directory
#
#####################################################################

def autorun_dir(*path):
	import os
	import sys;

	# Initialize
	directory = sys.path[0]
	import_path = ''

	# Join the path for the file and the import command
	for name in path:
		directory = os.path.join(directory, name)
		import_path = import_path + name + '.'

	# Get all files in the directory
	try:
		files = os.listdir(directory)
	except os.error:
		print "Can't list", directory
		files = []
	files.sort()

	# Filter out the relevant files
	# Only accept '*.py' and '*.pyc' files,
	# and include every module only once
	modules = {}
	for file in files:
		fullname = os.path.join(directory, file)
#		if os.path.isfile(fullname):
		head, tail = file[:-3], file[-3:]
		# XXX use imp.get_suffixes()
		if (tail in ('.py', '.pyc')) and (head[0] != '_'):
			modules[head] = None

	# Import the modules
	for module in modules.keys():
		__import__("%s%s" % (import_path, module))

