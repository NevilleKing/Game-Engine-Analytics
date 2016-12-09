#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class LogFile
{
public:
	LogFile(std::string FileName);
	~LogFile();
protected:
	std::vector<std::string> data;
};

