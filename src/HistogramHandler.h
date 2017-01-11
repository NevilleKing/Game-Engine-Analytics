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

	const GLfloat _quadColours[5][3] = {
		{ 1.0f, 0.54902f, 0.0f }, // dark orange
		{ 0.556863f, 0.219608f, 0.556863f }, // purple
		{ 0.0f, 1.0f, 0.498039f }, // green
		{ 1.0f, 0.411765f, 0.705882f }, // pink
		{ 0.698039f, 0.133333f, 0.133333f } // brick red
	};
};
