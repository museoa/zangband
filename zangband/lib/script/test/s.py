class test_server_class:
	def inkey_hook(self, key):
		global s
		s.send("%s" % key)

from socket import *
#HOST = '194.121.103.126'    # The remote host
HOST = '127.0.0.1'    # The remote host
PORT = 50000              # The same port as used by the server
s = socket(AF_INET, SOCK_STREAM)
s.connect(HOST, PORT)

from variable import events
events.inkey.append(test_server_class())
