import os

MYPATH = os.path.dirname(os.path.abspath(__file__))

for root, dirs, files in os.walk(MYPATH):
	for filename in files:
		if filename.endswith('.wav'):
			cmd = 'oggenc2.exe %s' % filename
			os.system(cmd)

