import os
import sys

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'usage:'
		print '  python touch.py file1 file2 ...'
		os.exit(1)

	for filename in sys.argv[1:]:
		os.utime(filename, None)

