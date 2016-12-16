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
private:
	std::vector<std::string> data;
	std::vector<glm::vec3> points;
	
	void parseData();
};

