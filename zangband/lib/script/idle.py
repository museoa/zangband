import sys
import os
path = os.path.join(sys.path[1], "lib-tk")
sys.path.insert(2, path)
path = os.path.join(sys.path[1], "idle")
sys.path.insert(2, path)

if 1:
	import python.idle.idle
else:
	try:
		import threading
		import python.idle.PyShell
		idle = threading.Thread(target=python.idle.PyShell.main)
		idle.start()
		import time
		time.sleep(2)
	except:
		import python.idle.idle

