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

	void Initialise(GLuint positionLocation);

	void render(GLuint colourLocation);

	std::vector<glm::vec2>* getVertexData() { return &_vertexData; }
	int getVertexDataSize() { return _vertexData.size(); }
private:
	// data structure to store bin (int) values - dynamic array of x . y elements
	std::vector<std::vector<int>> _bins;

	// max bin value
	int _maxBinValue = 0;

	// vertex data of the bins
	std::vector<glm::vec2> _vertexData;

	// int for VAO
	GLuint _vertextArrayObject;

	// holds if the VAO has been generated
	bool _isInit = false;

	// returns a colour value based on the value and the max bin value
	GLfloat* getBinColour(int binValue, GLfloat colour1[3], GLfloat colour2[3]);

	int _binY;
};
