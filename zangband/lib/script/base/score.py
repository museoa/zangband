######################################################################
#
# HighScore-file merger for Angband and ZAngband
#
# Version 1.1 (30th September 1999)
#
# Written by Robert Ruehlmann < rr9@angband.org >
#
# Combines two scorefiles into one, removes duplicate entries,
# sorts by scores (highest first)
#
# Usage: score.py scorefile1 scorefile2 outfile
#
######################################################################

import struct

class score_entry:
	def load(self, f):
		s = f.read(8)
		if s == "": return 0
		(self.what, ) = struct.unpack("8s", s)
		s = f.read(10)
		(self.pts, ) = struct.unpack("10s", s)
		s = f.read(10)
		(self.gold, ) = struct.unpack("10s", s)
		s = f.read(10)
		(self.turns, ) = struct.unpack("10s", s)
		s = f.read(10)
		(self.day, ) = struct.unpack("10s", s)
		s = f.read(16)
		(self.who, ) = struct.unpack("16s", s)
		s = f.read(8)
		(self.uid, ) = struct.unpack("8s", s)
		s = f.read(2)
		(self.sex, ) = struct.unpack("2s", s)
		s = f.read(3)
		(self.p_r, ) = struct.unpack("3s", s)
		s = f.read(3)
		(self.p_c, ) = struct.unpack("3s", s)
		s = f.read(4)
		(self.cu_lev, ) = struct.unpack("4s", s)
		s = f.read(4)
		(self.cu_dun, ) = struct.unpack("4s", s)
		s = f.read(4)
		(self.max_lev, ) = struct.unpack("4s", s)
		s = f.read(4)
		(self.max_dun, ) = struct.unpack("4s", s)
		s = f.read(32)
		(self.how, ) = struct.unpack("32s", s)

		return 1

	def save(self, f):
		s = struct.pack("8s", self.what)
		f.write(s)
		s = struct.pack("10s", self.pts)
		f.write(s)
		s = struct.pack("10s", self.gold)
		f.write(s)
		s = struct.pack("10s", self.turns)
		f.write(s)
		s = struct.pack("10s", self.day)
		f.write(s)
		s = struct.pack("16s", self.who)
		f.write(s)
		s = struct.pack("8s", self.uid)
		f.write(s)
		s = struct.pack("2s", self.sex)
		f.write(s)
		s = struct.pack("3s", self.p_r)
		f.write(s)
		s = struct.pack("3s", self.p_c)
		f.write(s)
		s = struct.pack("4s", self.cu_lev)
		f.write(s)
		s = struct.pack("4s", self.cu_dun)
		f.write(s)
		s = struct.pack("4s", self.max_lev)
		f.write(s)
		s = struct.pack("4s", self.max_dun)
		f.write(s)
		s = struct.pack("32s", self.how)
		f.write(s)

	def __cmp__(self, other):
		# Highest first
		if self.pts < other.pts:
			return 1
		elif self.pts > other.pts:
			return -1
		else:
			return 0

class scorefile:
	def __init__(self):
		self.scores = {}

	def load(self, name):
		f = open(name, "rb")

		while 1:
			score = score_entry()
			if score.load(f):
				key = (score.what, score.pts, score.gold, score.turns, score.day,
				       score.who, score.uid, score.sex, score.p_r, score.p_c,
				       score.cu_lev, score.cu_dun, score.max_lev, score.max_dun,
				       score.how)
				self.scores[key] = score
			else:
				break;

		f.close()

	def save(self, name):
		f = open(name, "wb")

		scores = self.scores.values()
		scores.sort()

		for score in scores:
			score.save(f)

		f.close()

	def merge(self, other):
		self.scores.update(other.scores)

# Here comes the "real" code :-)
def main():
	import sys
	args = sys.argv[1:]

	if len(args) <> 3:
		print "Not enough arguments"
		print "Usage: score.py scorefile1 scorefile2 outfile"
		return

	file1 = scorefile()
	file2 = scorefile()

	try:
		file1.load(args[0])
	except IOError, (errno, strerror):
		print args[0], ":", strerror
		return

	try:
		file2.load(args[1])
	except IOError, (errno, strerror):
		print args[1], ":", strerror
		return

	file1.merge(file2)

	try:
		file1.save(args[2])
	except IOError, (errno, strerror):
		print args[2], ":", strerror
		return

# Execute the code if run as script
if __name__ == '__main__':
	main()

