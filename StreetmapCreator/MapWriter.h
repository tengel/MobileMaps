#pragma once

#include <QString>
#include <QImage>

const int FORMAT_PNG = 0;
const int FORMAT_JPEG = 1;

bool WriteMap(QString outputFile, const QImage& im,
		  int tileSize, int overviewZoom, int format, bool usePNGCrush, QByteArray index);

