# usage:
#  python count_lines.py
#  python count_lines.py --details
#  python count_lines.py --details .h .hpp .cpp .cs

import os
import sys

DEFAULT_EXTS = ('.h', '.hpp', '.cpp')
flagDetails  = False

def fileLines(fname):
	i = 0 # empty file
	with open(fname) as f:
		for i, l in enumerate(f):
			pass
	return i + 1

def parseFlags():
	global DEFAULT_EXTS
	global flagDetails

	if len(sys.argv) == 1:
		return

	if sys.argv[1] == '--details':
		flagDetails = True
		if len(sys.argv[2:]) > 0:
			DEFAULT_EXTS = sys.argv[2:]
	else:
		DEFAULT_EXTS = sys.argv[1:]	

def detailWalk():
	extCounts = {}
	for ext in DEFAULT_EXTS:
		extCounts[ext] = {}

	dirname = os.path.dirname(os.path.abspath(__file__))
	for root, dirs, files in os.walk(dirname):
		for f in files:
			for ext in DEFAULT_EXTS:
				if f.endswith(ext):
					fname = os.path.join(root, f)
					count = fileLines(fname)

					currDir = root.replace(dirname, '')
					if not extCounts[ext].has_key(currDir):
						extCounts[ext][currDir] = 0
					extCounts[ext][currDir] += count

	# grab result
	# {'.h' : (totalExt, [('dir', count), ('dir', count), ...]), ... }
	result = {}
	total = 0
	for ext, detail in extCounts.iteritems():
		totalExt = 0

		dirDetails = []
		dirs = detail.keys()
		dirs.sort()	
		for currDir in dirs:
			count = detail[currDir]
	 		if count > 0:
				total += count
				totalExt += count
				dirDetails.append((currDir, count))

		if totalExt > 0:
			result[ext] = (totalExt, dirDetails)

	# print result
	for ext, t in result.iteritems():
		print '%s: %d' % (ext, t[0])
		for c in t[1]:
			print '  %s: %d' % c
	print 'total: %d' % total

def simpleWalk():
	extCounts = {}
	for ext in DEFAULT_EXTS:
		extCounts[ext] = 0

	dirname = os.path.dirname(os.path.abspath(__file__))
	for root, dirs, files in os.walk(dirname):
		for f in files:
			for ext in DEFAULT_EXTS:
				if f.endswith(ext):
					fname = os.path.join(root, f)
					count = fileLines(fname)
					extCounts[ext] += count

	total = 0
	for ext, count in extCounts.iteritems():
		if count > 0:
			total += count
			print '%s: %d' % (ext, count)
	print 'total: %d' % total

if __name__ == '__main__':
	parseFlags()

	if flagDetails:
		detailWalk()
	else:
		simpleWalk()

