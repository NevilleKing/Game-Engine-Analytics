#pragma once

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "LogFile.h"
#include "Histogram.h"

class HistogramHandler
{
public:
	HistogramHandler(GLuint positionLocation, GLuint colourLocation, const int binX, const int binY);
	~HistogramHandler();
	
	void addHistogram(LogFile* logFile);

	void render();
private:
	void Initialise();

	void allocateVertexBufferObject(GLsizeiptr size, const GLvoid* data);

	std::vector<Histogram*> _heatmaps;

	GLuint _positionLocation;
	GLuint _colourLocation;

	bool _isInit = false;

	// int for VAO
	GLuint _vertextArrayObject;

	// int for VBO
	GLuint _vertextBufferObject;

	int _binX = 0, _binY = 0;
};
