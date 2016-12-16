#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <glm\glm.hpp>

class LogFile
{
public:
	LogFile(std::string FileName);
	~LogFile();

	enum LogFileStatus { STATUS_OK, STATUS_ERROR };

	LogFileStatus getStatus() { return _status; };
private:
	std::vector<std::string> _data;
	std::vector<glm::vec3> points;

	// defined as error until complete
	LogFileStatus _status = STATUS_ERROR;
	
	void parseData();
};

