#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "LogFile.h"
#include "Histogram.h"

class HistogramHandler
{
public:
	HistogramHandler(GLuint positionLocation, GLuint colourLocation);
	~HistogramHandler();
	
	void addHistogram(LogFile* logFile, const int binX, const int binY);

	void render();
private:
	void Initialise();

	std::vector<Histogram> _heatmaps;

	GLuint _positionLocation;
	GLuint _colourLocation;

	bool _isInit = false;

	// int for VAO
	GLuint _vertextArrayObject;

	// int for VBO
	GLuint _vertextBufferObject;
};
