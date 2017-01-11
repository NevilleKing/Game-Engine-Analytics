#include "Histogram.h"

Histogram::Histogram(LogFile * logFile, const int binX, const int binY, glm::vec2 min, glm::vec2 max)
{
	lf = logFile;

	_min = min;
	_max = max;

	this->_binY = binY;
	this->_binX = binX;

	Initialise();
}

RGB Histogram::getBinColour(int binIndex)
{
	GLfloat binValue = _bins[binIndex / _binY][binIndex % _binY];

	// 0 - 1 based on bin value
	GLfloat val = (GLfloat)binValue / (GLfloat)_maxBinValue;

	GLfloat r, g, b;
	// using algorithm from http://stackoverflow.com/questions/13488957/interpolate-from-one-color-to-another
	r = (colour2[0] - colour1[0]) * val + colour1[0];
	g = (colour2[1] - colour1[1]) * val + colour1[1];
	b = (colour2[2] - colour1[2]) * val + colour1[2];

	RGB returnVal(r, g, b);
	returnVal.percent = val;
	return returnVal;
}

void Histogram::setColours(const GLfloat colour_2[3])
{
	colour2[0] = colour_2[0];
	colour2[1] = colour_2[1];
	colour2[2] = colour_2[2];
}

void Histogram::recalculate(glm::vec2 min, glm::vec2 max)
{
	if (lf != nullptr)
	{
		_vertexData.clear();
		_bins.clear();

		_min = min;
		_max = max;

		Initialise();
	}
}

void Histogram::Initialise()
{
	// initialise the vector
	for (int i = 0; i < _binX; i++)
	{
		std::vector<int> myBin;
		for (int j = 0; j < _binY; j++)
		{
			myBin.push_back(0);
		}
		_bins.push_back(myBin);
	}

	// find the ranges
	float rangeX = _max.x - _min.x;
	float rangeY = _max.y - _min.y;

	// find the bin width & height
	float binWidth = rangeX / (float)_binX;
	float binHeight = rangeY / (float)_binY;

	int sorted = 0, unsorted = 0;

	// loop through the data from the log file and sort into the bins
	for (auto point : lf->getData())
	{
		int cachedSort = sorted;
		// hold the current bin value
		glm::vec2 currentPosition(0, 0);
		while (currentPosition.y < _binY)
		{
			if (_min.x + (currentPosition.x * binWidth) <= point.x &&
				_min.x + (currentPosition.x * binWidth) + binWidth >= point.x &&
				_min.y + (currentPosition.y * binHeight) <= point.y &&
				_min.y + (currentPosition.y * binHeight) + binHeight >= point.y)
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
			if (currentPosition.x >= _binX)
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
	for (int i = 0; i < _binX; i++)
	{
		// loop through y
		for (int j = 0; j < _binY; j++)
		{
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth, _min.y + j*binHeight)); // top left
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth + binWidth, _min.y + j*binHeight)); // top right
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth, _min.y + j*binHeight + binHeight)); // bottom left
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth + binWidth, _min.y + j*binHeight + binHeight)); // bottom right
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth + binWidth, _min.y + j*binHeight)); // top right
			_vertexData.push_back(glm::vec2(_min.x + i*binWidth, _min.y + j*binHeight + binHeight)); // bottom left
		}
	}
}
