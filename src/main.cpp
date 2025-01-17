// h264p.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include <conio.h>
#endif
#include <iostream>
#include <fstream>
#include "TestH264Parser.h"

using namespace std;

const int N = 1024;

#ifdef _WIN32
#define fprintf fprintf_s
#endif

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "usage: avc2json infile" << endl;
        return -1;
    }

    ifstream ifile(argv[1], std::ios::in | std::ios::binary);
    if (!ifile)
    {
        printf("Error: Fail to open file %s", argv[1]);
        return -1;
    }

    uint8_t memblock[N]{};
    try
    {
        clock_t start, end;
        start = clock();

        avc2json::TestH264Parser p(argv[1]);

        while (ifile.good())
        {
            ifile.read((char*)memblock, N);
            p.parse(memblock, (unsigned)ifile.gcount());
        }

        end = clock();
        double dif = (double)(end - start) / CLOCKS_PER_SEC;
        fprintf(stderr, "Elapsed time is %2.3lf seconds.", dif);

        ifile.close();
    }
    catch (...)
    {
        cerr << "Unknown exception thrown\n";
    }

    return 0;
}

