#####################################################################
#
# init.py
#
# Initialization script
#
#####################################################################

try:
	# Init a *basic* error handler (so that we can debug the real one)
	import util.error
	util.error.error_handler()

	import vars

	from util import autorun
	autorun.autorun_dir("startup")
except:
	import traceback
	traceback.print_exc()
