#pragma once

#include <iostream>
#include <vector>
#include "LogFile.h"
#include <GL/glew.h>

class Histogram 
{
public:
	// load the log file data into the histogram
	Histogram(LogFile* logFile, const int binX, const int binY);

	glm::vec2* getVertexData() { return &_vertexData[0]; }
	int getVertexDataBufferSize() { return _vertexData.size() * sizeof(&_vertexData[0]); }
	int getVertexDataSize() { return _vertexData.size(); }

	// returns a colour value based on the value and the max bin value
	GLfloat* getBinColour(int binIndex);
private:
	// data structure to store bin (int) values - dynamic array of x . y elements
	std::vector<std::vector<int>> _bins;

	// max bin value
	int _maxBinValue = 0;

	// vertex data of the bins
	std::vector<glm::vec2> _vertexData;

	// holds if the VAO has been generated
	bool _isInit = false;

	int _binY;

	const GLfloat colour1[3] = { 0.0f, 0.0f, 0.0f };
	const GLfloat colour2[3] = { 1.0f, 0.54902f, 0.0f };
};
