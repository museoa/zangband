#####################################################################
# plugin.py
#####################################################################

#####################################################################
# class plugin
#####################################################################

class plugin:
	# Name of the plug-in
	name = "empty plug-in"

	# Name of the author(s) - can be used as key in a "contact database"
	# where infos about developers (like e-mail adress, webpage, ...)
	# are stored centrally for the game.  That makes updating such
	# infos easier (for example when changing the ISP).
	author = "nobody"

	# Version number
	version = 0

	# Release-date of this version
	release_date = None

	# Description
	description = "This plug-in does nothing."

	# Link to the webpage of the plug-in
	link = "http://thangorodrim.angband.org/"

	def __init__(self):
		self.active = 0

	def install(self):
		pass

	def uninstall(self):
		pass

	# Activate the plug-in
	def activate(self):
		self.active = 1
		# Copy initialization files to the startup folder

	# Deactivate the plug-in
	def deactivate(self):
		self.active = 0
		# Remove initialization files from the startup folder

	def configure(self):
		pass

	def test(self):
		pass

