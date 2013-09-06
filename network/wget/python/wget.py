# python wget.py [-v|--verbose] <url>
import sys
import urllib2

def downloadSimple(url, filename):
	u = urllib2.urlopen(url)
	f = open(filename, 'wb')
	print 'Downloading: %s' % filename

	blockSize = 8192
	while True:
		buf = u.read(blockSize)
		if not buf:
			break

		f.write(buf)

	f.close()

def downloadVerbose(url, filename):
	u = urllib2.urlopen(url)
	f = open(filename, 'wb')
	meta = u.info()
	filesize = int(meta.getheaders('Content-Length')[0])
	print 'Downloading: %s Bytes: %s' % (filename, filesize)

	filesizeDownload = 0
	blockSize = 8192
	while True:
		buf = u.read(blockSize)
		if not buf:
			break

		filesizeDownload += len(buf)
		f.write(buf)
		status = r"%10d [%3.2f%%]" % (filesizeDownload, filesizeDownload*100./filesize)
		status = status + chr(8)*(len(status)+1)
		print status,

	f.close()

def usageAndExit():
	print 'python wget.py [-v|--verbose] <url>'
	sys.exit(0)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		usageAndExit()

	url = sys.argv[-1]
	if not url.startswith('http'):
		usageAndExit()

	temp = url.split('/')
	if len(temp) == 1:
		usageAndExit()

	filename = temp[-1]
	if '-v' in sys.argv or '--verbose' in sys.argv:
		downloadVerbose(url, filename)
	else:
		downloadSimple(url, filename)

