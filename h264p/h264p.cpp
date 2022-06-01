// h264p.cpp : Defines the entry point for the console application.
//

#include <conio.h>
#include <iostream>
#include <fstream>
#include "TestH264Parser.h"

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

using namespace std;

const int N = 1024;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cerr << "usage: h264p infile" << endl;
		return -1;
	}

	ifstream ifile(argv[1],std::ios::in|std::ios::binary);
	if(!ifile)
	{
		printf("Error: Fail to open file %s", argv[1]);
		return -1;
	}

	char memblock[N]{};
	try
	{
		clock_t start, end;
		start = clock();

		TestH264Parser p(argv[1]);

		while(ifile.good())
		{
			ifile.read((char*)memblock,N);
			p.parse(memblock,(unsigned)ifile.gcount());
		}

		end = clock();
		double dif = (double)(end - start) / CLOCKS_PER_SEC;
		fprintf_s(stderr,"Elapsed time is %2.3lf seconds.", dif);

		ifile.close();
	}
	catch(...)
	{
		cerr << "Unknown exception thrown\n";
	}

	/*cerr << endl << "Enter any key to exit" << endl;
	_getch();*/
	return 0;
}

