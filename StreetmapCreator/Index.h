#pragma once

#include <QString>
#include <QByteArray>
#include <QList>

/** This is the information stored for each item. */
struct Record
{
	QString title;
	int x;
	int y;
};


/** A branch in the digit tree. Adds one more digit. Also contains the records
 * associated with the current number title. */
class DigitItem
{
public:
	DigitItem();
	DigitItem(const QString& numTitleSoFar, QChar d); // The digit.
	~DigitItem();
	
	// The total length in bytes of this DigitItem and its child records and DigitItems.
	int length() const;

	QChar digit() const;

	// The data to write.
	QByteArray data() const;
	
	// Add a record. atDigit keeps track of where we are up to.
	void addRecord(const Record& R, int atDigit = 0);
	
	// The number of child/this DigitItem's that contain records.
	int containedUniqueRecords() const;

private:
	// What this digit is.
	QChar theDigit;
	// Child digits.
	QList<DigitItem> children;
	// The number title.
	QString numTitle;
	// The records for this numTitle. There may be none.
	QList<Record> records;
	// Length cache. -1 = invalidated.
	mutable int lengthCache;
	// Contained Unique Digits cache. -1 = invalidated.
	mutable int containedUniqueRecordsCache;
};

void writeInt(QByteArray& s, int i); // 4 Bytes
void writeShort(QByteArray& s, int i); // 2 Bytes.
void writeString(QByteArray& s, const QString& str);
void writeChar(QByteArray& s, QChar c);
void writeByte(QByteArray& s, int b); // byte is anded with FF.
void writeBCDString(QByteArray& s, const QString& digits); // BCD string. The low nibble has the first character. Ended by 0xF? or 0xFF.

class Index
{
public:
	Index();
	~Index();
	
	void add(const QString& title, int x, int y);
	
	QByteArray data();
	
private:	
	// The root item. character is not important for this one.
	DigitItem items;
};
