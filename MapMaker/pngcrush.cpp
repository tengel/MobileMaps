#include "pngcrush.h"

#include <QByteArray>
#include <QTemporaryFile>
#include <QProcess>

bool PNGCrushPresent()
{
	QProcess crush;
	crush.start("pngcrush");
	return crush.waitForStarted(1000);
}

bool PNGCrush(QByteArray& im)
{
	// We have to write the data to a file.
	QTemporaryFile inFile;
	if (!inFile.open())
		return false;

	QString inFileName = inFile.fileName();

	if (inFile.write(im) == -1)
		return false;
	inFile.close();

	QTemporaryFile outFile;
	if (!outFile.open())
		return false;

	QString outFileName = outFile.fileName();

	outFile.close();

	QProcess crush;
	crush.start("pngcrush", QStringList() << "-cc" << inFileName << outFileName);
	if (!crush.waitForFinished())
		return false;

	if (crush.exitCode() != 0 || crush.exitStatus() != QProcess::NormalExit)
		return false;

	if (!outFile.open())
		return false;

	QByteArray ba = outFile.readAll();
	if (ba.size() == 0)
		return false;

	im = ba;
	return true;
}
