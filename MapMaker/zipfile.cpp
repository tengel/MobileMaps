#include "zipfile.h"

#include <QFile>

ZipFile::ZipFile() : zf(NULL)
{
}
ZipFile::~ZipFile()
{
	Close();
}

// Append a file to a zip file. The zipFile must have been already opened. fileName is the name that the file
// will be given within the zip file. ba is the data that will be in the file.
bool ZipFile::AppendFile(QString fileName, const QByteArray& ba, QString comment)
{
	if (!zf)
		return false;

	zip_fileinfo zi;
	memset(&zi, 0, sizeof(zi));

	int r = zipOpenNewFileInZip(zf,
								fileName.toAscii().constData(),
								&zi,
								NULL,
								0,
								NULL,
								0,
								comment.toAscii().constData(),
								Z_DEFLATED,
								9); // Compression level (9=max)
	if (r != ZIP_OK)
		return false;

	r = zipWriteInFileInZip(zf, ba.constData(), ba.size());
	if (r != ZIP_OK)
		return false;

	r = zipCloseFileInZip(zf);
	if (r != ZIP_OK)
		return false;

	return true;
}

// Overloaded function where the data is read from a file on disk.
bool ZipFile::AppendFile(QString fileName, QString fileNameOnDisk, QString comment)
{
	if (!zf)
		return false;

	QFile file(fileNameOnDisk);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QByteArray ba = file.readAll();

	return AppendFile(fileName, ba, comment);
}

bool ZipFile::Open(QString fileName) // TODO: Change this to a class.
{
	Close();

	// Open the jar (it's really a zip) file:
	zf = zipOpen(fileName.toAscii().constData(), APPEND_STATUS_ADDINZIP);
	if (!zf)
		return false;

	return true;
}

bool ZipFile::Close(QString comment)
{
	if (!zf)
		return false;
	bool success = zipClose(zf, comment.toAscii().constData()) == ZIP_OK;
	zf = NULL;
	return success;
}
