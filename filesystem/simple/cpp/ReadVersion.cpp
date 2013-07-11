#include <stdio.h>
#include <assert.h>
#include <string>

void usage()
{
	printf("usage: ReadVersion.exe <README>\n");
}

class LineReader
{
public:
	LineReader(const char* filename)
	{
		fp_ = fopen(filename, "r");
	}

	~LineReader()
	{
		if (fp_)
		{
			fclose(fp_);
			fp_ = NULL;
		}
	}

	bool isEOF()
	{
		if (!fp_)
			return true;

		return feof(fp_);
	}

	std::string readLine()
	{
		assert(fp_);

		std::string s;
		int c;
	
		while (1)
		{
			c = fgetc(fp_);
			if (c == EOF)
				break;
	
			if (c == '\r')
			{
				c = fgetc(fp_);
				if (c == EOF)
					break;
				else if (c != '\n')
					fputc(c, fp_);
	
				break;
			}
	
			if (c == '\n')
			{
				break;
			}
	
			s.push_back(static_cast<char>(c));
		}
	
		return s;
	}

private:
	FILE *fp_;
};

namespace strutils
{

bool startsWith(const std::string& s, const std::string& subs)
{
	size_t n = subs.length();
	if (n > s.length())
		return false;

	std::string tmp = s.substr(0, n);
	return tmp == subs;
}

};

std::string readVersion(const char* filename)
{
	std::string ver;
	LineReader lr(filename);
	while (!lr.isEOF())
	{
		std::string line = lr.readLine();
		if (strutils::startsWith(line, "Version="))
		{
			ver = line.substr(8);
			break;
		}
	}
	return ver;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		usage();
		return 1;
	}

	std::string ver = readVersion(argv[1]);
	printf("%s\n", ver.c_str());

	return 0;
}

