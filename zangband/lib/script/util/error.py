#####################################################################
#
# util/error.py
#
# Print error messages and log them
#
#####################################################################


import sys
from ioc import msg_print
import os.path
from angband import system

class error_handler:
	def write(self, text):
		if text:
			filename = os.path.join(system.cvar.ANGBAND_DIR_SCRIPT, "error.txt")
			error_file = open(filename, "a")
			error_file.write(text)
			error_file.close()

			if text[-1] == '\n':
				msg_print(text[0:-1])
			else:
				msg_print(text)

# Redirect stderr, stdout, and stdin
sys.stderr = error_handler()

