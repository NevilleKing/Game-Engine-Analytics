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
}
