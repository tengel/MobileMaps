#ifndef FILESTOREWRITER_H
#define FILESTOREWRITER_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QFile>

class FileStoreWriter
{
public:
    FileStoreWriter();
	void appendFile(const QByteArray& ba);
	bool writeToFile(QString filename);
private:

	bool writeJavaInt(QFile& f, int i);

	QList<QByteArray> files;
};

#endif // FILESTOREWRITER_H
