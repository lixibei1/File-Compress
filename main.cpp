#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include "huffman.h"
#include "file.cpp"
using namespace std;
void TestFileCompress()
{

	FileCompress fc;
	FileCompress fc1;
	

	fc.Compress("./1.txt");
	fc1.uncompress("1.txt.huffman");

}
int main()
{
	TestFileCompress();
	system("pause");
	return 0;
}


	



