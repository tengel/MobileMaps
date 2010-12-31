#include "Index.h"

QString ToNumbers(const QString& title);

DigitItem::DigitItem()
{
	theDigit = '\0';
	lengthCache = -1;
	containedUniqueRecordsCache = -1;
}
DigitItem::DigitItem(const QString& numTitleSoFar, QChar d)
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

QChar DigitItem::digit() const
{
	return theDigit;
}

QByteArray DigitItem::data() const
{
	QByteArray dat;

	if (theDigit == '\0')
	{
		// This is a special case for the root DigitItem.	
		for (int i = 0; i < children.size(); ++i)
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
			QByteArray test;
			writeBCDString(test, numTitle);
			int recordsLength = 1 + 4 + test.length();
			for (int i = 0; i < records.size(); ++i)
				recordsLength += records[i].title.length() + 1 + 4 + 4;

			writeByte(dat, records.size());
			writeInt(dat, recordsLength);
			writeBCDString(dat, numTitle);
			for (int i = 0; i < records.size(); ++i)
			{
				writeString(dat, records[i].title);
				writeInt(dat, records[i].x);
				writeInt(dat, records[i].y);
			}
		}
		
		for (int i = 0; i < children.size(); ++i)
			dat += children[i].data();
	}
	return dat;
}

void DigitItem::addRecord(const Record& R, int atDigit)
{
	lengthCache = -1;
	containedUniqueRecordsCache = -1;
	
	QString recordNumTitle = ToNumbers(R.title);
	
	if (atDigit >= recordNumTitle.length())
	{
		// OK add it here.
		records.append(R); // TODO: Some kind of sorting.
	}
	else
	{
		// Search for the letter equal to numTitle[atDigit].
		int i;
		for (i = 0; i < children.size(); ++i)
		{
			if (children[i].digit() == recordNumTitle[atDigit])
			{
				children[i].addRecord(R, ++atDigit);
				return;
			}
			if (children[i].digit() > recordNumTitle[atDigit])
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
		for (int i = 0; i < children.size(); ++i)
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

void Index::add(const QString& title, int x, int y)
{
	Record R;
	R.title = title;
	R.x = x;
	R.y = y;
	items.addRecord(R);
}

QByteArray Index::data()
{
	QByteArray dat = items.data();
	writeByte(dat, 0xff);
	return dat;
}

/** *************************/

QString ToNumbers(const QString& title)
{
	QString ret;
	QByteArray asciiTitle = title.toLower().toAscii();
	for (int i = 0; i < asciiTitle.length(); ++i)
	{
		switch(asciiTitle[i])
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

QChar ItoC(int i)
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

int CtoI(QChar c)
{
	switch (c.toAscii())
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

/** *************************/

void writeInt(QByteArray& s, int i)
{
//	cout << "Int: " << i << endl;
	int be = ((i >> 24) & 0x000000FF) | ((i >> 8) & 0x0000FF00) | ((i << 8) & 0x00FF0000) | ((i << 24) & 0xFF000000);
	s.append(reinterpret_cast<char*>(&be), 4);
}

void writeShort(QByteArray& s, int b)
{
	int be = ((b << 8) & 0x0000FF00) | ((b >> 8) & 0x000000FF);
	s.append(reinterpret_cast<char*>(&be), 2);
}
void writeString(QByteArray& s, const QString& str)
{
//	cout << "String: " << str << endl;
	s.append(str.toAscii());
	s.append('\0');
}
void writeChar(QByteArray& s, QChar c)
{
//	cout << "Char: " << c << endl;
	s.append(c.toAscii());
}
void writeByte(QByteArray& s, int b) // byte is anded with FF.
{
//	cout << "Byte: " << (b & 0xFF) << endl;
	s.append(reinterpret_cast<char*>(&b), 1); // Little endian host.
}
void writeBCDString(QByteArray& s, const QString& digits) // BCD string. The low nibble has the first character. Ended by 0xF? or 0xFF.
{
//	cout << "BCD String: " << digits << endl;
	unsigned char low = 0xFF;
	for (int i = 0; i < digits.size(); ++i)
	{
		if (digits[i].isDigit())
		{
			if (low == 0xFF)
			{
				low = CtoI(digits[i]);
			}
			else
			{
				s.append(low | (CtoI(digits[i]) << 4));
				low = 0xFF;
			}
		}
	}
	s.append(low | 0xF0);
}
