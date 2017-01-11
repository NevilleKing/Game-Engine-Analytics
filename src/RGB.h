#pragma once

#include "GL/glew.h"

class RGB
{
public:
	GLfloat r;
	GLfloat g;
	GLfloat b;

	RGB(GLfloat red, GLfloat green, GLfloat blue)
	{
		r = red;
		g = green;
		b = blue;
	};

	RGB() {}
};
