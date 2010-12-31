#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

/** This is the information stored for each article. */
struct Record
{
	string title;
	unsigned short x;
	unsigned short y;
};


/** A branch in the digit tree. Adds one more digit. Also contains the records
 * associated with the current number title. */
class DigitItem
{
public:
	DigitItem();
	DigitItem(const string& numTitleSoFar, char d); // The digit.
	~DigitItem();
	
	// The total length in bytes of this DigitItem and its child records and DigitItems.
	int length() const;

	char digit() const;

	// The data to write.
	string data() const;
	
	// Add a record. atDigit keeps track of where we are up to.
	void addRecord(const Record& R, unsigned int atDigit = 0);
	
	// The number of child/this DigitItem's that contain records.
	int containedUniqueRecords() const;

private:
	// What this digit is.
	char theDigit;
	// Child digits.
	vector<DigitItem> children;
	// The number title.
	string numTitle;
	// The records for this numTitle. There may be none.
	vector<Record> records;
	// Length cache. -1 = invalidated.
	mutable int lengthCache;
	// Contained Unique Digits cache. -1 = invalidated.
	mutable int containedUniqueRecordsCache;
};

void writeInt(string& s, int i); // 4 Bytes
void writeUShort(string& s, unsigned short i); // 2 Bytes
void writePackedInt(string& s, int i); // 1-5 Bytes
void writeString(string& s, const string& str);
void writeChar(string& s, char c);
void writeByte(string& s, int b); // byte is anded with FF.
void writeBCDString(string& s, const string& digits); // BCD string. The low nibble has the first character. Ended by 0xF? or 0xFF.

class Index
{
public:
	Index();
	~Index();
	
	void add(const string& title, unsigned short x, unsigned short y);
	
	void write(const string& file) const;

	string write() const;
	
private:	
	// The root item. character is not important for this one.
	DigitItem items;
};
