class test_client_class:
	def inkey_borg_hook(self, key):
		try:
			global conn
			#if scan:
			#	conn.setblocking(0)
			#else:
			#	conn.setblocking(1)
			a = conn.recv(1)
			if a:
				#print ord(a[0])
				return ord(a[0])
			else:
				conn.close()
				print "Connection closed"
		except:
			pass

from socket import *
HOST = ''                 # Symbolic name meaning the local host
PORT = 50000              # Arbitrary non-privileged server
s = socket(AF_INET, SOCK_STREAM)
s.bind(HOST, PORT)
s.listen(1)
conn, addr = s.accept()
#conn.setblocking(0)

from variable import events
events.inkey_borg.append(test_client_class())

