#####################################################################
#
# Generic code for player-sex
#
#####################################################################


import angband.prace

#####################################################################
# Base class for all player-genders
#####################################################################
class player_sex(angband.prace.player_sex):
	version = 0
	title = ""
	winner = ""

	def __init__(self):
		# Init the C data
		angband.prace.player_sex.__init__(self)

		# HACK - Set all values
		# This works since __getattr__() is only triggered for undefined
		# attributes while __setattr__() is always used
		self.title = self.title
		self.winner = self.winner

	def __str__(self):
		return self.title

	# Make sure the C data is reinitialized when unpickling
	def __getinitargs__(self):
		return ()

