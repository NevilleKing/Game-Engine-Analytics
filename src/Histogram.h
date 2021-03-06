#pragma once

#include <iostream>
#include <vector>
#include "LogFile.h"
#include <GL/glew.h>
#include "RGB.h"

class Histogram 
{
public:
	// load the log file data into the histogram
	Histogram(LogFile* logFile, const int binX, const int binY, glm::vec2 min, glm::vec2 max);
	Histogram(LogFile* logFile, const int binX, const int binY) : Histogram(logFile, binX, binY, logFile->getMin(), logFile->getMax()) {};

	glm::vec2* getVertexData() { return &_vertexData[0]; }
	int getVertexDataBufferSize() { return _vertexData.size() * sizeof(&_vertexData[0]); }
	int getVertexDataSize() { return _vertexData.size(); }

	// returns a colour value based on the value and the max bin value
	RGB getBinColour(int binIndex);

	glm::vec2 getMin() { return _min; }
	glm::vec2 getMax() { return _max; }

	void setColours(const GLfloat colour2[3]);

	void recalculate(glm::vec2 min, glm::vec2 max);
private:
	// data structure to store bin (int) values - dynamic array of x . y elements
	std::vector<std::vector<int>> _bins;

	// max bin value
	int _maxBinValue = 0;

	// vertex data of the bins
	std::vector<glm::vec2> _vertexData;

	// holds if the VAO has been generated
	bool _isInit = false;

	int _binY, _binX;

	GLfloat colour1[3] = { 0.0f, 0.0f, 0.0f };
	GLfloat colour2[3] = { 1.0f, 0.54902f, 0.0f };

	glm::vec2 _min;
	glm::vec2 _max;

	LogFile* lf;

	void Initialise();
};
