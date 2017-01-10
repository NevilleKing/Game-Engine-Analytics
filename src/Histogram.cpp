#include "Histogram.h"

Histogram::Histogram(LogFile * logFile, const int binX, const int binY)
{
	// initialise the vector
	for (int i = 0; i < binX; i++)
	{
		std::vector<int> myBin;
		for (int j = 0; j < binY; j++)
		{
			myBin.push_back(0);
		}
		_bins.push_back(myBin);
	}

	// get max and min from the log file
	glm::vec2 max = logFile->getMax();
	glm::vec2 min = logFile->getMin();

	// find the ranges
	float rangeX = max.x - min.x;
	float rangeY = max.y - min.y;

	// find the bin width & height
	float binWidth = rangeX / (float)binX;
	float binHeight = rangeY / (float)binY;

	// loop through the data from the log file and sort into the bins
	for (auto point : logFile->getData())
	{
		// hold the current bin value
		glm::vec2 currentPosition(0, 0);
		while (currentPosition.y < binY)
		{
			if ((currentPosition.x * binWidth) <= point.x &&
				(currentPosition.x * binWidth) + binWidth >= point.x &&
				(currentPosition.y * binHeight) <= point.y &&
				(currentPosition.y * binHeight) + binHeight >= point.y)
			{
				// in current bin
				_bins[(int)currentPosition.x][(int)currentPosition.y]++;
				int newBinValue = _bins[(int)currentPosition.x][(int)currentPosition.y];
				if (newBinValue > _maxBinValue)
					_maxBinValue = newBinValue;
				break;
			}

			currentPosition.x++;
			if (currentPosition.x >= binX)
			{
				currentPosition.x = 0;
				currentPosition.y++;
			}
		}
	}

	// calculate the vertex data points for each of the bins
	// loop through x
	for (int i = 0; i < binX; i++)
	{
		// loop through y
		for (int j = 0; j < binY; j++)
		{
			_vertexData.push_back(glm::vec2(min.x+i*binWidth, min.y + j*binHeight)); // top left
			_vertexData.push_back(glm::vec2(min.x + i*binWidth+binWidth, min.y + j*binHeight)); // top right
			_vertexData.push_back(glm::vec2(min.x + i*binWidth, min.y + j*binHeight+binHeight)); // bottom left
			_vertexData.push_back(glm::vec2(min.x + i*binWidth+binWidth, min.y + j*binHeight+binHeight)); // bottom right
			_vertexData.push_back(glm::vec2(min.x + i*binWidth + binWidth, min.y + j*binHeight)); // top right
			_vertexData.push_back(glm::vec2(min.x + i*binWidth, min.y + j*binHeight + binHeight)); // bottom left
		}
	}

	this->_binY = binY;
}

void Histogram::Initialise(GLuint positionLocation)
{
	if (!_isInit)
	{
		_isInit = true;

		// Vertex Buffer
		GLuint VDBO;

		glGenBuffers(1, &VDBO);

		glBindBuffer(GL_ARRAY_BUFFER, VDBO);
		glBufferData(GL_ARRAY_BUFFER, _vertexData.size() * sizeof(&_vertexData), &_vertexData[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		std::cout << "vertexDataBufferObject created OK! GLUint is: " << VDBO << std::endl;

		// Vertex Array Object

		GLuint VAO; // hold the current value for the VAO

		glGenVertexArrays(1, &VAO); //create a Vertex Array Object

		glBindVertexArray(VAO); //make the just created vertexArrayObject the active one

		glBindBuffer(GL_ARRAY_BUFFER, VDBO); //bind vertexDataBufferObject

		glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation

		glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation

		glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

							  //cleanup
		glDisableVertexAttribArray(positionLocation); //disable vertex attribute at index positionLocation
		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

		std::cout << "Vertex Array Object created OK! GLUint is: " << VAO << std::endl;

		_vertextArrayObject = VAO;
	}
}

void Histogram::render(GLuint colorLocation)
{
	if (_isInit)
	{
		glBindVertexArray(_vertextArrayObject);

		int binIndex = 0;
		for (int i = 0; i < _vertexData.size(); i += 6)
		{
			GLfloat* color = getBinColour(_bins[binIndex / _binY][binIndex % _binY]);
			glUniform3f(colorLocation, color[0], color[1], color[2]);
			glDrawArrays(GL_TRIANGLES, i, 12);
			binIndex++;
		}

		glBindVertexArray(0);
	}
}

GLfloat * Histogram::getBinColour(int binValue)
{
	/*
		Colour mapping : http://stackoverflow.com/questions/12875486/what-is-the-algorithm-to-create-colors-for-a-heatmap
		0    : blue   rgb(0, 0, 1)
		0.25 : cyan   rgb(0, 1, 1)
		0.5  : green  rgb(0, 1, 0)
		0.75 : yellow rgb(1, 1, 0)
		1    : red    rgb(1, 0, 0)
	*/

	// 0 - 1 based on bin value
	GLfloat val = (GLfloat)binValue / (GLfloat)_maxBinValue;

	GLfloat r = val;
	GLfloat g = 0;
	GLfloat b = 0;

	GLfloat returnVal[] = { r, g, b };
	return returnVal;
}
