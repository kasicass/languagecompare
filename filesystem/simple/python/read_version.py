import sys

def readVersion(filename):
	f = open(filename)
	for line in f.xreadlines():
		line = line.strip()
		if line.startswith('Version='):
			return line.split('=')[1]
	return None

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'usage: python read_version.py <README>'
		sys.exit(1)

	version = readVersion(sys.argv[1])
	print version

