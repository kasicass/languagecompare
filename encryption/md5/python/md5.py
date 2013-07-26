import os
import sys
import hashlib

def usage():
	print 'usage:'
	print '  python md5.py -s <string>'
	print '  python md5.py <filename>'

def filemd5(filename):
	f = open(filename, 'rb')
	buf = f.read(8192)
	m = hashlib.md5()
	while len(buf) > 0:
		m.update(buf)
		buf = f.read(8192)
	f.close()
	return m.hexdigest()

def stringmd5(s):
	m = hashlib.md5()
	m.update(s)
	return m.hexdigest()

if __name__ == '__main__':
	if len(sys.argv) == 2:
		print filemd5(sys.argv[1])	
	elif len(sys.argv) == 3 and sys.argv[1] == '-s':
		print stringmd5(sys.argv[2])
	else:
		usage()
	
