#include "Index.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

string ToNumbers(const string& title);


DigitItem::DigitItem()
{
	theDigit = '\0';
	lengthCache = -1;
	containedUniqueRecordsCache = -1;
}
DigitItem::DigitItem(const string& numTitleSoFar, char d)
{
	theDigit = d;
	numTitle = numTitleSoFar + d;
	lengthCache = -1;
	containedUniqueRecordsCache = -1;
}
DigitItem::~DigitItem()
{
	
}

int DigitItem::length() const
{
	if (lengthCache == -1)
	{
		lengthCache = 0; // Prevent recursion!
		lengthCache = data().length();
	}
	
	return lengthCache;
}

char DigitItem::digit() const
{
	return theDigit;
}

string DigitItem::data() const
{
	string dat;

	if (theDigit == '\0')
	{
		// This is a special case for the root DigitItem.	
		for (unsigned int i = 0; i < children.size(); ++i)
			dat += children[i].data();
	}
	else
	{
		// This is for all normal DigitItems.
		if (containedUniqueRecords() > 1)
		{
			writeByte(dat, 0); // Indicate that this is a digit.
			writeInt(dat, length());
			writeChar(dat, theDigit);
		}
		
		if (records.size() > 0)
		{
			// Get the length.
			string test;
			writeBCDString(test, numTitle);
			int recordsLength = 1 + 4 + test.length();
			for (unsigned int i = 0; i < records.size(); ++i)
				recordsLength += records[i].title.length() + 1 + 4 + 4;

			writeByte(dat, records.size());
			writeInt(dat, recordsLength);
			writeBCDString(dat, numTitle);
			for (unsigned int i = 0; i < records.size(); ++i)
			{
				writeString(dat, records[i].title);
				writeInt(dat, records[i].x);
				writeInt(dat, records[i].y);
				// TODO: writeTwoWords.
			}
		}
		
		for (unsigned int i = 0; i < children.size(); ++i)
			dat += children[i].data();
	}
	return dat;
}

void DigitItem::addRecord(const Record& R, unsigned int atDigit)
{
	lengthCache = -1;
	containedUniqueRecordsCache = -1;
	
	string recordNumTitle = ToNumbers(R.title);
	
	if (atDigit >= recordNumTitle.length())
	{
		// OK add it here.
		records.push_back(R); // TODO: Some kind of sorting.
	}
	else
	{
		// Search for the letter equal to numTitle[atDigit].
		vector<DigitItem>::iterator i;
		for (i = children.begin(); i != children.end(); ++i)
		{
			if (i->digit() == recordNumTitle[atDigit])
			{
				i->addRecord(R, ++atDigit);
				return;
			}
			if (i->digit() > recordNumTitle[atDigit])
			{
				break;
			}
		}
		DigitItem newDigit(numTitle, recordNumTitle[atDigit]);
		newDigit.addRecord(R, ++atDigit);
		children.insert(i, newDigit);
	}
}

int DigitItem::containedUniqueRecords() const
{
	if (containedUniqueRecordsCache == -1)
	{
		containedUniqueRecordsCache = records.size() > 0 ? 1 : 0;
		for (unsigned int i = 0; i < children.size(); ++i)
			containedUniqueRecordsCache += children[i].containedUniqueRecords();
	}
	return containedUniqueRecordsCache;
}





/** *************************/

Index::Index()
{
}
Index::~Index()
{
}

string ToNumbers(const string& title)
{
	string ret;
	for (unsigned int i = 0; i < title.length(); ++i)
	{
		switch(tolower(title[i]))
		{
			case 'a':
			case 'b':
			case 'c':
			case '2':
				ret += '2';
				break;
			case 'd':
			case 'e':
			case 'f':
			case '3':
				ret += '3';
				break;
			case 'g':
			case 'h':
			case 'i':
			case '4':
				ret += '4';
				break;
			case 'j':
			case 'k':
			case 'l':
			case '5':
				ret += '5';
				break;
			case 'm':
			case 'n':
			case 'o':
			case '6':
				ret += '6';
				break;
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case '7':
				ret += '7';
				break;
			case 't':
			case 'u':
			case 'v':
			case '8':
				ret += '8';
				break;
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case '9':
				ret += '9';
				break;
			case ' ':
			case '0':
				ret += '0';
				break;
			case '1':
			default:
				ret += '1';
				break;
		}
	}
	return ret;
}

char ItoC(int i)
{
	switch (i)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	}
	return '1';
}

int CtoI(char c)
{
	switch (c)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	}
	return '1';	
}

void Index::add(const string& title, int x, int y)
{
	Record R;
	R.title = title;
	R.x = x;
	R.y = y;
	items.addRecord(R);
}

void Index::write(const string& file)
{
	/* Index file format example.
	 *
	 * Simple to start with - just store the offsets to [000]-[999] in ints.
	 * The [0]-[9] and [00]-[99] are stored first.
	 */

	ofstream of(file.c_str(), ios::out | ios::binary);
	if (!of)
		return;

	string dat = items.data();
	writeByte(dat, 0xff);
	of << dat;
}
	
void writeInt(string& s, int i)
{
//	cout << "Int: " << i << endl;
	int be = ((i >> 24) & 0x000000FF) | ((i >> 8) & 0x0000FF00) | ((i << 8) & 0x00FF0000) | ((i << 24) & 0xFF000000);
	s += string(reinterpret_cast<char*>(&be), 4);
}

void writeTwoWords(string& s, int a, int b)
{
	int be = ((a << 24) & 0xFF000000) | ((a << 8) & 0x00FF0000) | ((b << 8) & 0x0000FF00) | ((b >> 8) & 0x000000FF);
	s += string(reinterpret_cast<char*>(&be), 4);
}
void writePackedInt(string& s, int i)
{
	abort();
	while (i & 0xFF)
	{
		// TODO
	}
}
void writeString(string& s, const string& str)
{
//	cout << "String: " << str << endl;
	s += str + '\0';
}
void writeChar(string& s, char c)
{
//	cout << "Char: " << c << endl;
	s += c;
}
void writeByte(string& s, int b) // byte is anded with FF.
{
//	cout << "Byte: " << (b & 0xFF) << endl;
	s += string(reinterpret_cast<char*>(&b), 1); // Little endian host.
}
void writeBCDString(string& s, const string& digits) // BCD string. The low nibble has the first character. Ended by 0xF? or 0xFF.
{
//	cout << "BCD String: " << digits << endl;
	unsigned char low = 0xFF;
	for (unsigned int i = 0; i < digits.size(); ++i)
	{
		if (isdigit(digits[i]))
		{
			if (low == 0xFF)
			{
				low = CtoI(digits[i]);
			}
			else
			{
				s += low | (CtoI(digits[i]) << 4);
				low = 0xFF;
			}
		}
	}
	s += low | 0xF0;
}
