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

	int sorted = 0, unsorted = 0;

	// loop through the data from the log file and sort into the bins
	for (auto point : logFile->getData())
	{
		int cachedSort = sorted;
		// hold the current bin value
		glm::vec2 currentPosition(0, 0);
		while (currentPosition.y < binY)
		{
			if (min.x + (currentPosition.x * binWidth) <= point.x &&
				min.x + (currentPosition.x * binWidth) + binWidth >= point.x &&
				min.y + (currentPosition.y * binHeight) <= point.y &&
				min.y + (currentPosition.y * binHeight) + binHeight >= point.y)
			{
				// in current bin
				_bins[(int)currentPosition.x][(int)currentPosition.y]++;
				int newBinValue = _bins[(int)currentPosition.x][(int)currentPosition.y];
				if (newBinValue > _maxBinValue)
					_maxBinValue = newBinValue;
				sorted++;
				break;
			}

			currentPosition.x++;
			if (currentPosition.x >= binX)
			{
				currentPosition.x = 0;
				currentPosition.y++;
			}
		}
		if (sorted == cachedSort)
			unsorted++;
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

GLfloat * Histogram::getBinColour(int binIndex)
{
	GLfloat binValue = _bins[binIndex / _binY][binIndex % _binY];

	// 0 - 1 based on bin value
	GLfloat val = (GLfloat)binValue / (GLfloat)_maxBinValue;

	GLfloat r, g, b;
	// using algorithm from http://stackoverflow.com/questions/13488957/interpolate-from-one-color-to-another
	r = (colour2[0] - colour1[0]) * val + colour1[0];
	g = (colour2[1] - colour1[1]) * val + colour1[1];
	b = (colour2[2] - colour1[2]) * val + colour1[2];

	GLfloat returnVal[] = { r, g, b };
	return returnVal;
}
