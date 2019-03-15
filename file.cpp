#define _CRT_SECURE_NO_WARNINGS 1
#pragma once
#include"huffman.h"
#include<iostream>
#include<vector>
using namespace std;
#include<assert.h>

typedef long long LongType;

struct CharInfo
{
	char _ch;//�ַ�
	LongType _count;//����
	string  _code;//����

	bool operator !=(const CharInfo&info)
	{
		return _count != info._count;
	}
	CharInfo operator+(const CharInfo&info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}
	bool operator<(const CharInfo&info)
	{
		return _count > info._count;
	}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	struct TmpInfo
	{
		char _ch;//�ַ�
		LongType _count;//����
	};
public:
	//���캯��
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}
	//��ȡ����������
	void GenerateHuffmanCode(Node*root, string code)//code���ܴ�����??
	{
		if (root == NULL)
			return;
		//ǰ��������ɱ���
		if (root->_pLeft == NULL&&root->_pRight == NULL)
		{
			_infos[(unsigned char)root->_weight._ch]._code = code;
			return;
		}
		GenerateHuffmanCode(root->_pLeft, code + '0');
		GenerateHuffmanCode(root->_pRight, code + '1');
	}
	void Compress(const char *file)//file:Դ�ļ�
	{
		//1.ͳ���ַ����ֵĴ���
		printf("%s", file);
		FILE*fout = fopen(file, "rb");
		assert(fout);
		char ch = fgetc(fout);
		while (ch != EOF || feof(fout) == 0)//���ļ��������򷵻�ֵΪ1������Ϊ0
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
		//2.����Huffmantree ��code
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo>tree(_infos, 256, invalid);//���������飬256������Чֵ������0�Σ�

		string compressfile = file;//
		compressfile += ".huffman";//?
		FILE* fin = fopen(compressfile.c_str(), "wb");//��ѹ���ļ�
		assert(fin);


		string code;
		GenerateHuffmanCode(tree.GetRoot(), code);

		//3.0д���ַ����ֵ���Ϣ
		//fwrite(_infos, sizeof(CharInfo), 256, fin);
		int writeNum = 0;
		int objSize = sizeof(TmpInfo);
		for (rsize_t i = 0; i < 256; ++i)
		{
			if (_infos[i]._count>0)
			{
				TmpInfo info;
				info._ch = _infos[i]._ch;
				info._count = _infos[i]._count;
				fwrite(&info, objSize, 1, fin);
				writeNum++;
			}
		}
		TmpInfo info;
		info._count = -1;
		fwrite(&info, objSize, 1, fin);//��info._count = -1д��ȥ��Ϊ������־λ

		//3.ѹ��
		fseek(fout, 0, SEEK_SET);//�ļ�ָ�롢ƫ����������λ��
		ch = fgetc(fout);
		char value = 0;
		size_t pos = 0;
		while (ch != EOF)
		{
			string &code = _infos[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '1')
					value |= (1 << pos);
				else if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}
				else
				{
					assert(false);
				}
				++pos;
				if (pos == 8)
				{
					fputc(value, fin);
					value = 0;
					pos = 0;
				}

			}
			ch = fgetc(fout);
		}
		if (pos > 0)
		{
			fputc(value, fin);//д��ѹ���ļ���fin��
		}
		fclose(fout);
		fclose(fin);
	}
	void uncompress(const char *file)
	{
		string uncompressfile = file;//file:Input.txt.huffman
		size_t pos = uncompressfile.rfind('.');//�ҵ�������һ��'.'
		assert(pos != string::npos);
		uncompressfile.erase(pos);//ɾ����'.'�����ַ���
		uncompressfile += ".unhuffman";//Input.txt+'.unhuffman'
		FILE*fin = fopen(uncompressfile.c_str(), "wb");//�򿪽�ѹ���ļ�
		assert(fin);
		FILE*fout = fopen(file, "rb");//��ѹ���ļ�
		assert(fout);
		//fread(_infos, sizeof(CharInfo), 256, fout);
		//3.0�����ַ����ֵ���Ϣ
		TmpInfo info;
		int cycleNum = 1;
		int objSize = sizeof(TmpInfo);
		fread(&info, objSize, 1, fout);

		while (info._count != -1)//-1Ϊ������־
		{
			_infos[(unsigned char)info._ch]._ch = info._ch;
			_infos[(unsigned char)info._ch]._count = info._count;

			fread(&info, objSize, 1, fout);
			cycleNum++;
		}

		int aaa = 0;
		//�ؽ�huaffman��
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo>tree(_infos, 256, invalid);//���������飬256������Чֵ������0�Σ�
		Node *root = tree.GetRoot();
		Node*cur = root;
		LongType n = root->_weight._count;//����Ҷ�ӽڵ�ĺͣ�Դ�ļ��ַ��ĸ�����
		char ch = fgetc(fout);//��fout(ѹ���ļ�)���ַ�
		while (ch != EOF || n>0)
		{
			for (size_t i = 0; i < 8; ++i)
			{

				if ((ch&(1 << i)) == 0)
					cur = cur->_pLeft;
				else
					cur = cur->_pRight;
				if (cur->_pLeft == NULL&&cur->_pRight == NULL)
				{
					//cout << cur->_weight._ch;
					fputc(cur->_weight._ch, fin);//fin��ѹ���ļ�
					cur = root;
					if (--n == 0)
						break;
				}

			}
			ch = fgetc(fout);
		}
		fclose(fin);
		fclose(fout);

	}
protected:
	CharInfo _infos[256];
};
