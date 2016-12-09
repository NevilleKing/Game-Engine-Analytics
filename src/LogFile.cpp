#include "LogFile.h"

LogFile::LogFile(std::string FileName)
{
	// open the file
	std::ifstream theFile;
	theFile.open(FileName);
	
	// error checking
	if (!theFile.is_open())
		return;

	// read each line one by one
	std::string line;
	while (std::getline(theFile, line))
	{
		data.push_back(line);
	}

	// close the file
	theFile.close();
}

LogFile::~LogFile()
{
}
