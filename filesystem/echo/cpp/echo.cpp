#include <sstream>
#include <iostream>

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 0;

	int i;
	std::stringstream ss;
	for (i = 1; i < argc-1; i++)
	{
		ss << argv[i] << " ";
	}
	ss << argv[i];

	std::cout << ss.str() << std::endl;
	return 0;
}
