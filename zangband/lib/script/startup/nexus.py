from base.utils import proxy

# Make "Nexus" known
import vars
vars.worlds.register(proxy("Nexus", "worlds.nexus", "nexus"))

