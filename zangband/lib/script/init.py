#####################################################################
#
# init.py
#
# Initialization script
#
#####################################################################

try:
	import util.error
	import variable
	from util import autorun
	autorun.autorun_dir("startup")
except:
	import traceback
	traceback.print_exc()

