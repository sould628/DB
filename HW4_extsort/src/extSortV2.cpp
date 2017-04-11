#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <string>
#include <exception>

#define PAGE_SIZE 100
#define MEM_SIZE 2
#define BUF_SIZE (PAGE_SIZE * MEM_SIZE)

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::sort;
using std::stringstream;

void mad() {
    throw std::exception();
}

void sortAndWrite(int* buf, int len, int fileID) {
    sort(buf, buf + len);
    stringstream ss1, ss2;
    ss1 << "output_binary/temp_0_" << fileID;
    ofstream out(ss1.str().c_str(), ios::out | ios:: binary);
    if (!out.is_open()) {
        cout << "failed to open output file" << endl;
        cout << "make sure that the directory \"output_binary/\" exists \n" << endl;
        mad();
    }
    out.write((char*) buf, sizeof(int) * len);
    out.close();

    ss2 << "output_debug/temp_0_" << fileID;
    ofstream out_debug(ss2.str().c_str(), ios::out);
    for (int j = 0; j < len; ++j)
        out_debug << buf[j] << endl;
    out_debug.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "usage: <input>" << endl;
        return 1;
    }
    int fileID = 0;
    int* buf = new int[BUF_SIZE];
    ifstream in(argv[1], ios::in | ios::binary);
    if (!in.is_open()) {
        cout << "failed to open input file" << endl;
        mad();
    }

    while (in) {
        in.read((char*) buf, sizeof(int) * BUF_SIZE);
        int len = in.gcount() / sizeof(int);
        if (len > 0) {
            sortAndWrite(buf, len, fileID);
            ++fileID;
        }
    }
    // ASSUME that the input size is always PAGE_SIZE * 2^n
    // Then, the buffer is always full
    in.close();
    delete[] buf;

    int* buf1 = new int[PAGE_SIZE];
    int* buf2 = new int[PAGE_SIZE];
    int nRun = 0;

    // ASSUME that the input size is always PAGE_SIZE * 2^n
    // Then, the number of files is always 2^n
    for (int i = fileID; i > 1; i /= 2) {
        int j = 0; // input file index
        int c = 0; // output file index
        while (j < i) {
            stringstream ss1, ss2, ss3, ss4;
            ss1 << "output_binary/temp_" << (nRun + 1) << "_" << c;
            ss2 << "output_debug/temp_" << (nRun + 1) << "_" << (c++);
            ofstream out(ss1.str().c_str(), ios::out | ios::binary);
            ofstream out_debug(ss2.str().c_str(), ios::out);
            ss3 << "output_binary/temp_" << nRun << "_" << (j++);
            ss4 << "output_binary/temp_" << nRun << "_" << (j++);
            ifstream in1(ss3.str().c_str(), ios::in | ios::binary);
            ifstream in2(ss4.str().c_str(), ios::in | ios::binary);

            // TODO: merge-sort and output
            // USE: in1.read((char*) (buf1 + k), sizeof(int));
            // USE: out.write((char*) &value, sizeof(int));
            // USE: out_debug << value << endl;

			//buf1: inputbuffer
			//buf2: outputbuffer
			cout << "sorting pass " << nRun + 1 << endl;

			int k1 = 0, k2 = 0; //indices
			int numInput1Read = 0;
			int numInput2Read = 0;
			bool in1Finished = false, in2Finished = false;
			bool time2read1 = true, time2read2 = true;
			int value;
			while (!in1Finished || !in2Finished)
			{
				if (!in1Finished&&time2read1)
				{
					if (numInput1Read++ == pow(2, nRun + 1))
					{
						in1Finished = true;
						cout << "finished in1" << endl;
					}
					else
					{
						k1 = 0;
						in1.read((char*) buf1, sizeof(int)*PAGE_SIZE);
					}
					time2read1 = false;
				}
				if (!in2Finished&&time2read2)
				{
					if (numInput2Read++ == pow(2, nRun + 1))
					{
						in2Finished = true;
						cout << "finished in2" << endl;
					}
					else
					{
						in2.read((char*) buf2, sizeof(int)*PAGE_SIZE);
						k2 = 0;
					}
					time2read2 = false;
				}
				while ((!time2read1&&!time2read2)&&(!in1Finished || !in2Finished))
				{
					if (in1Finished)	value = buf2[k2++];
					else if (in2Finished)	value = buf1[k1++];
					else
					{
						if (buf1[k1] <= buf2[k2])
						{
							value = buf1[k1++];
						}
						else
						{
							value = buf2[k2++];
						}
					}
					if ((k1 == PAGE_SIZE)&&!in1Finished)
					{
						time2read1 = true;
					}
					if ((k2 == PAGE_SIZE)&&!in2Finished)
					{
						time2read2 = true;
					}
					out.write((char*) &value, sizeof(int));
					out_debug << value << endl;
					cout << value<<endl;
				}
			}
			//for (int ii = 0; ii < PAGE_SIZE; ii++)
			//{
			//	cout << buf1[ii] << endl;
			//}
			//int t;
			//std::cin >> t;

			////////////////////////////////////////////////////
            in1.close();
            in2.close();
            out.close();
            out_debug.close();
        }
        ++nRun;
    }
    delete[] buf1;
    delete[] buf2;

    return 0;
}
