#pragma once

#include <iostream>
#include <vector>
#include "LogFile.h"

class Histogram 
{
public:
	// load the log file data into the histogram
	Histogram(LogFile* logFile, const int binX, const int binY);

private:
	// data structure to store bin (int) values - dynamic array of x . y elements
	std::vector<std::vector<int>> _bins;
};
