#pragma once

#include "glm\glm.hpp"

class Line
{
public:
	Line(glm::vec2 start, glm::vec2 end);

	glm::vec2 startPoint;
	glm::vec2 endPoint;
};
