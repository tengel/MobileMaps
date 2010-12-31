#pragma once

#include "zlib/minizip/zip.h"

#include <QString>

class ZipFile
{
public:
	ZipFile();
	~ZipFile();

	bool Open(QString fileName);
	bool AppendFile(QString fileName, const QByteArray& ba, QString comment = "");
	bool AppendFile(QString fileName, QString fileNameOnDisk, QString comment = "");
	bool Close(QString comment = "");
private:
	const ZipFile& operator=(const ZipFile&);
	ZipFile(const ZipFile&);

	zipFile zf;
};
