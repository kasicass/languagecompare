import sys

if __name__ == '__main__':
	if len(sys.argv) < 2:
		sys.exit(1)

	print ' '.join(sys.argv[1:])
