#include "HistogramHandler.h"

HistogramHandler::HistogramHandler(GLuint positionLocation, GLuint colourLocation)
{
	_positionLocation = positionLocation;
	_colourLocation = colourLocation;
}

HistogramHandler::~HistogramHandler()
{
}

void HistogramHandler::addHistogram(LogFile* logFile, const int binX, const int binY)
{
	_heatmaps.push_back(Histogram(logFile, binX, binY));
	_heatmaps[0].Initialise(_positionLocation);
}

void HistogramHandler::render()
{
	_heatmaps[0].render(_colourLocation);
}
