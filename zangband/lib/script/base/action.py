#####################################################################
#
# base/action.py
#
# Base class for actions
#
#####################################################################


#####################################################################
#
# Base class for actions
#
#####################################################################
class action:
	"""Add a description of the function and it's arguments here."""

	# Name of the class
	name = "action"

	# Execute an action
	def execute(self, **args):
		pass

	# Show the configuration wizard and return the arguments as string
	def wizard(self):
		pass

	# Return a string that imports ourself when executed
	def get_import(self):
		return "from base.action import action"



class fireball(action):
	"""Creates a fireball

	source = source coordinates of the ball (y, x tuple)
	target = target coordinates of the ball (y, x tuple)
	radius = radius of the ball
	damage = damage at the center of the ball"""

	name = "fireball"

	# Fire the ball
	def execute(self, **args):
		print self.name, args["source"], args["target"], args["radius"], args["damage"]

	# Return a string that imports ourself when executed
	def get_import(self):
		return "from base.action import fireball"

