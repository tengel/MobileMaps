#pragma once

#include <QString>

const int FORMAT_PNG = 0;
const int FORMAT_JPEG = 1;

bool Make(QString midletName, QString inputImage, QString inputIndex, QString outputFile,
		  int tileSize, int overviewZoom, int format, QString iconFileName, bool usePNGCrush, bool writeJADFile);

int EstimateSize(QString inputImage, int tileSize, int overviewZoom, int format);
