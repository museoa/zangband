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

class ErrorClass:
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
	def read(self, n = -1):
		pass

# Redirect stderr and stdout
sys.stderr = sys.stdout = ErrorClass()

