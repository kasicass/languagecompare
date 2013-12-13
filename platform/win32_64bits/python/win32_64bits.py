# http://stackoverflow.com/questions/2764356/python-get-windows-os-version-and-architecture
# 
# @rem Test environment using this table:
# @rem
# @rem Environment Variable       32bit Native    64bit Native    WOW64
# @rem PROCESSOR_ARCHITECTURE     x86             AMD64           x86
# @rem PROCESSOR_ARCHITEW6432     undefined       undefined       AMD64

import os

def win32_64bits():
	a = os.environ.get('PROCESSOR_ARCHITECTURE', '')
	if a == 'x86':
		b = os.environ.get('PROCESSOR_ARCHITEW6432', '')
		if b == 'AMD64':
			return '64bits'
		else:
			return '32bits'
	elif a == 'AMD64':
		return '64bits'
	else:
		return 'unkown'
		
if __name__ == '__main__':
	print 'OS is', win32_64bits()

