#include "LogFile.h"

LogFile::LogFile(std::string FileName)
{
	try
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
			_data.push_back(line);
		}

		// close the file
		theFile.close();

		// run the parsing function
		parseData();
	}
	catch (int e) {}
}

LogFile::~LogFile()
{
}

void LogFile::parseData()
{
	// loop through all the data in the '_data' vector
	for (auto line : _data)
	{
		// data is in the format x,y,z on each line
		// covert the string to a string stream (required by getline function)
		std::stringstream ss(line); // http://www.cplusplus.com/reference/sstream/stringstream/stringstream/
		
		// split the data by ','
		std::string split_value;
		int i = 0;
		float vecs[3];
		while (std::getline(ss, split_value, ',')) // http://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
		{
			if (i > 2) break;
			vecs[i++] = ::atof(split_value.c_str()); // http://stackoverflow.com/questions/1012571/stdstring-to-float-or-double
		}

		// construct a glm vec3 object
		glm::vec3 position(vecs[0], vecs[1], vecs[2]);

		// save to points vector
		points.push_back(position);
	}

	// change status
	_status = LogFileStatus::STATUS_OK;
}
