#include "FileStoreWriter.h"

#include <QFile>

FileStoreWriter::FileStoreWriter()
{
}

void FileStoreWriter::appendFile(const QByteArray& ba)
{
	files.append(ba);
}
bool FileStoreWriter::writeToFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	if (!writeJavaInt(file, files.size()))
		return false;
	int offset = files.size() * 4 + 4;
	for (int i = 0; i < files.size(); ++i)
	{
		if (!writeJavaInt(file, offset))
			return false;
		offset += files[i].size() + 4; // 4 extra bytes for the length.
	}
	for (int i = 0; i < files.size(); ++i)
	{
		if (!writeJavaInt(file, files[i].size()))
			return false;
		if (!file.write(files[i]))
			return false;
	}
	file.close();
	return true;
}

bool FileStoreWriter::writeJavaInt(QFile& f, int i)
{
	int j = ((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | ((i << 8) & 0xFF0000) | ((i << 24) & 0xFF000000);
	return f.write(reinterpret_cast<char*>(&j), 4) == 4;
}
