#####################################################################
#
# util/debug.py
#
# Debugging
#
#####################################################################

import os.path
from angband import system
from python import string

class debug:
	trace_switch = 0

	def trace(self, text):
		if self.trace_switch:
			filename = os.path.join(system.cvar.ANGBAND_DIR_SCRIPT, "trace.txt")
			trace_file = open(filename, "a")
			trace_file.write(text + '\n')
			trace_file.close()

