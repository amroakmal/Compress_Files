// Huffman.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif //  _MSC_VER

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <cstddef> // for std::size_t -> is a typedef on an unsinged int
#include <queue>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <cassert>
#include <bitset>
#include <algorithm>
using namespace std;

class Node
{
public:
	Node *left;
	Node *right;
	char data;
	unsigned int freq;

public:
	Node(char data, unsigned int freq, Node *left, Node *right)
	{
		this->data = data;
		this->freq = freq;
		this->left = left;
		this->right = right;
	}
	Node()
	{
	}
};

typedef pair<unsigned int, Node *> pi;

class HuffmanTree
{
public:
	Node *rootNode;
	map<char, string> codes;

public:
	HuffmanTree() {}
	HuffmanTree(vector<pair<unsigned int, char>> &FrequencyArray)
	{
		Node *left;
		Node *right;
		Node *top;
		vector<Node *> Nodes(FrequencyArray.size());
		priority_queue<pi, vector<pi>, greater<pi>> pq;
		for (int i = 0; i < FrequencyArray.size(); i++)
		{
			Node *temp = (Node *)malloc(sizeof(Node));
			temp->data = FrequencyArray[i].second;
			temp->freq = FrequencyArray[i].first;
			temp->left = NULL;
			temp->right = NULL;
			Nodes[i] = temp;
			pq.push(make_pair(FrequencyArray[i].first, temp));
		}
		while (pq.size() != 1)
		{
			pi temp1 = pq.top();
			pq.pop();
			pi temp2 = pq.top();
			pq.pop();
			left = temp1.second;
			right = temp2.second;
			top = (Node *)malloc(sizeof(Node));
			top->data = '$';
			top->freq = temp1.first + temp2.first;
			top->left = left;
			top->right = right;
			pq.push(make_pair(top->freq, top));
		}
		rootNode = pq.top().second;
		pq.pop();
		vector<int> arr(1000);
		int topnum = 0;
		GetCodes(rootNode, arr, topnum);
	}

	void GetCodes(Node *root, vector<int> &arr, int top)
	{
		if (root->left)
		{
			arr[top] = 0;
			GetCodes(root->left, arr, top + 1);
		}
		if (root->right)
		{
			arr[top] = 1;
			GetCodes(root->right, arr, top + 1);
		}
		if (root->left == NULL && root->right == NULL)
		{
			string code = "";
			for (int i = 0; i < top; i++)
			{
				code += std::to_string(arr[i]);
			}
			codes.insert(pair<char, string>(root->data, code));
		}
	}
};

class Compressor
{
public:
	string path;
	int length;
	HuffmanTree hf;
	char *thefile;

public:
	Compressor(string path)
	{
		this->path = path;
		vector<pair<unsigned int, char>> farray;
		vector<int> allbytes;
		thefile = readFile(farray, &length);
		HuffmanTree hftemp(farray);
		hf = hftemp;
	}
	void createFarray(vector<pair<unsigned int, char>> &farray, vector<int> &freqarray)
	{
		for (int i = 0; i < freqarray.size(); i++)
		{
			if (freqarray[i] > 0)
			{
				farray.push_back(make_pair(freqarray[i], char(i)));
			}
		}
	}

	char *readFile(vector<pair<unsigned int, char>> &farray, int *length)
	{
		ifstream inFile;
		size_t size = 0;
		vector<int> freqarray(256, 0);
		inFile.open(path, ios::binary);

		inFile.seekg(0, ios::end);
		size = inFile.tellg();
		inFile.seekg(0, ios::beg);
		char *oData = (char *)(malloc(size + 1));
		oData[size] = '\0';
		inFile.read(oData, size);
		*(length) = size;
		for (size_t i = 0; i < size; i++)
		{
			int temp2 = int(oData[i]) & 255;
			freqarray[temp2]++;
		}
		createFarray(farray, freqarray);
		return oData;
	}

	void writetofile(string res, string path)
	{
		string res2 = "";
		//for (std::map<char, string>::iterator it = hf.codes.begin(); it != hf.codes.end(); it++) {
		//	res2 += it->first + std::to_string(it->second.size()) + it->second;
		//}
		fstream fout(path, fstream::app);
		res2 += to_string(length) + (char)32;
		res2 += to_string(hf.codes.size()) + (char)32;
		for (std::map<char, string>::iterator it = hf.codes.begin(); it != hf.codes.end(); it++)
		{
			res2 += it->first + it->second + (char)32;
		}
		fout.write(res2.c_str(), res2.size());
		char buffer[((int) res.size())];
		// assert(((int) res.size() % 8) == 0);
		int index = 0;
		for (int i = 0; i < (int) res.size(); i += 8)
		{
			int temp = 0;
			for (int j = i; j < i + 8; j++)
			{
				temp = temp << 1;
				if (j < res.size() && res.at(j) == '1')
				{
					temp = temp | 1;
				}
			}
			buffer[index] = temp;
			index++;
			//res2 = res2 + (char)temp;
			//unsigned char byte = (unsigned char) Integer
		}
		fout.write(buffer, index);
		//ofstream fout;
		//fout.open(path, ios::binary | ios::out);
		//cout << res2 << endl;
		//fout.write(res2.c_str(), res2.size());
	}

	string concatzeros(string res)
	{
		while (res.size() % 8 != 0)
			res = res + "0";
		return res;
	}

	void Compress()
	{
		//std::string filestring(thefile, length);
		string res = "";
		int templength = length;
		for (int i = 0; i < templength; i++)
		{
			char temp = thefile[i];
			res += hf.codes.find(thefile[i])->second;
		}
		//res = concatzeros(res);
		writetofile(res, "Compressed_" + path);
	}

	void printCodes()
	{
		for (std::map<char, string>::iterator it = hf.codes.begin(); it != hf.codes.end(); it++)
		{
			cout << (int)it->first << "-->" << it->first << "-->" << it->second << endl;
		}
	}
};

class Decompressor
{
public:
	string path;
	unordered_map<string, char> codes;
	int length;
	char *oData;
	int maxcodelength;

public:
	Decompressor(string path)
	{
		this->path = path;
		maxcodelength = 0;
	}

	string toBinaryString(char c)
	{
		int numerical_char = (int) c;
		bitset<8> b(numerical_char);
		return b.to_string();
	}

	int FromStrtoInt(string str)
	{
		int x;
		stringstream toint(str);
		toint >> x;
		return x;
	}

	int RetrieveCodes()
	{
		string numberofcharsstr = "";
		string numberofcodesstr = "";
		int numberofchars = 0;
		int numberofcodes = 0;
		ifstream inFile;
		size_t size = 0;
		inFile.open(path, ios::binary);
		inFile.seekg(0, ios::end);
		size = inFile.tellg();
		inFile.seekg(0, ios::beg);
		oData = (char *)(malloc(size));
		inFile.read(oData, size);
		int i = 0;
		while (oData[i] != (char)32)
		{
			numberofcharsstr += oData[i];
			i++;
		}
		numberofchars = FromStrtoInt(numberofcharsstr);
		i++;
		while (oData[i] != (char)32)
		{
			numberofcodesstr += oData[i];
			i++;
		}
		numberofcodes = FromStrtoInt(numberofcodesstr);
		i++;
		for (int j = 0; j < numberofcodes; j++)
		{
			char symbol = oData[i];
			i++;
			string code = "";
			while (oData[i] != (char) 32)
			{
				code += oData[i];
				i++;
			}
			if ((int) code.size() > maxcodelength)
			{
				maxcodelength = code.size();
			}
			codes[code] = symbol;
			i++;
		}

		oData += i;
		length = numberofchars;
		return size;
	}

	bool Has_Equivalent_char(string code)
	{
		auto it = codes.find(code);
		if (it != codes.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Decompress()
	{
		int size = RetrieveCodes();
		ofstream fout("Decompressed_" + path ,std::ofstream::binary);
		string code = "";

		int numchars = length, i = 0, codelength = 1;
		while (numchars != 0)
		{
			while ((int) code.size() < maxcodelength && i < size)
			{
				code += toBinaryString(oData[i]);
				i++;
			}
			if (Has_Equivalent_char(code.substr(0, codelength)))
			{
				fout.put(codes.find(code.substr(0, codelength))->second);
				code.erase(0, codelength);
				numchars--;
				codelength = 1;
			}
			else
			{
				codelength++;
			}
		}
	}
};

int main()
{
	string path;
	cout << "Enter the Path:" << endl;
	cout << "-->";
	cin >> path;
	bool encode = false;
	unsigned short int choice;
	cout << "To Encode Enter 1" << endl;
	cout << "To Decode Enter 0" << endl;
	cout << "-->";
	cin >> choice;
	encode = (choice == 1);
	if (encode)
	{
		Compressor c(path);
		c.Compress();
		//c.printCodes();
		cout << c.length << endl;
	}
	else
	{
		Decompressor d(path);
		d.Decompress();

	}
}