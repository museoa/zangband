#####################################################################
#
# util/debug.py
#
# Debugging
#
#####################################################################

import sys
import os.path
from angband import system, io
from python import string
from angband.io import msg_print, get_line


# Maximum length of the script-window buffer
max_text_length = 2048


class debug_class:
	trace_switch = 0

	def __init__(self):
		self._text = ""
		import sys
		sys.stdin = sys.stdout = sys.stderr = self
#		sys.settrace(self.trace_dispatch)


	def trace_dispatch(self, frame, event, arg):
		filename = os.path.join(system.cvar.ANGBAND_DIR_SCRIPT, "trace.txt")
		trace_file = open(filename, "a")

		filename = frame.f_code.co_filename
		lineno = frame.f_lineno

		trace_file.write("%s %s %s %s\n" % (event, filename, lineno, arg))
		trace_file.close()
		return self.trace_dispatch


	def trace(self, text):
		if self.trace_switch:
			filename = os.path.join(system.cvar.ANGBAND_DIR_SCRIPT, "trace.txt")
			trace_file = open(filename, "a")
			trace_file.write(text + '\n')
			trace_file.close()


	def write(self, text):
		filename = os.path.join(system.cvar.ANGBAND_DIR_SCRIPT, "error.txt")
		error_file = open(filename, "a")
		error_file.write(text)
		error_file.close()

		try:
			terms = self.get_debug_terms()
	
			if not terms:
				if text[-1] == '\n':
					msg_print(text[0:-1])
				else:
					msg_print(text)
	
			self._text = self._text + text
	
			self._text = self._text[-max_text_length:]
	
			self.refresh()
		except:
			# Fallback to the basic error-handler
			import error
			error.error_handler()


	def read(self, n = -1):
		pass


	def get_debug_terms(self):
		debug_terms = []

		# Scan windows
		for i in range(0, 7):
			# No window
			if not io.cvar.angband_term[i]:
				continue

			# No relevant flags
			if not (io.cvar.window_flag[i] & (io.PW_SCRIPT)):
				continue

			debug_terms.append(i)

		return debug_terms

	def readline(self):
		# Save old window
		old = io.cvar.Term

		terms = self.get_debug_terms()

		# Activate
		if terms:
			io.Term_activate(io.cvar.angband_term[terms[0]])

		# Get text
		text = self.get_input()

		# Restore
		io.Term_activate(old)

		return text


	def get_input(self):
		text = ''
		done = 0
		k = 0

		# Cursor
		err, x, y = io.Term_locate()

		len = 80 - x

		while not done:
			# Place cursor
			io.Term_gotoxy(x + k, y)

			# XXX
			# io.cvar.inkey_base = 1

			# Get a key
			key = io.inkey()

			# Return
			if key in "\n\r":
				done = 1
			# Escape
			elif ord(key) == 27:
				done = 1
				k = 0
				text = ""
			# Backspace
			elif ord(key) == 8:
				if k > 0:
					k = k - 1
					text = text[:-1]
					io.Term_erase(x, y, len)
					io.Term_putstr(x, y, -1, io.TERM_WHITE, text)
				else:
					io.bell()
			else:
				text = text + key
				k = k + 1
				io.Term_erase(x, y, len)
				io.Term_putstr(x, y, -1, io.TERM_WHITE, text)

		self.write(text + '\n')
		
		return text + '\n'


	def get_script_window_line_hook(self, line):
		return ""
		if 0:
			if line < len(self._text):
				text = self._text[line]
			else:
				text = ""
			return text


	def refresh_term(self):
		# Get size
		err, w, h = io.Term_get_size()

		lines = string.split(self._text, '\n')

		display = []

		# Add the new lines (with automatic line-wrapping)
		for line in lines:
			while len(line) > w:
				display.insert(0, line[0:w])
				line = line[w:]
			display.insert(0, line)

			# Remove old lines
			display = display[:h]

		# Dump messages
		for i in range(0, h):
			if i < len(display):
				text = display[i]
			else:
				text = ""
			# Dump the message on the appropriate line
			io.Term_putstr(0, (h - 1) - i, -1, io.TERM_WHITE, text)

			# Cursor
			err, x, y = io.Term_locate()

			# Clear to end of line
			io.Term_erase(x, y, 255)

		# Place the cursor
		io.Term_gotoxy(len(display[0]), h - 1)

		# Fresh
		io.Term_fresh()


	def refresh(self):
		# Save old window
		old = io.cvar.Term

		terms = self.get_debug_terms()

		# Scan windows
		for term in terms:
			# Activate
			io.Term_activate(io.cvar.angband_term[term])

			# Refresh
			self.refresh_term()

		# Restore
		io.Term_activate(old)
