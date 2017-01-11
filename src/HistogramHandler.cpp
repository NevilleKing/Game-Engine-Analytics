#include "HistogramHandler.h"

HistogramHandler::HistogramHandler(GLuint positionLocation, GLuint colourLocation, const int binX, const int binY)
{
	_positionLocation = positionLocation;
	_colourLocation = colourLocation;
	_binX = binX;
	_binY = binY;
}

HistogramHandler::~HistogramHandler()
{
	for (auto heatmap : _heatmaps)
		delete heatmap;

	glDeleteBuffers(1, &_vertextBufferObject);
	glDeleteVertexArrays(1, &_vertextArrayObject);
}

void HistogramHandler::addHistogram(LogFile* logFile)
{
	if (_heatmaps.size() == 0)
	{
		_heatmaps.push_back(new Histogram(logFile, _binX, _binY));
		_heatmaps.back()->setColours(_quadColours[_heatmaps.size() - 1]);
		Initialise();
		_currentMin = logFile->getMin();
		_currentMax = logFile->getMax();
	}
	else
	{		
		// if the new heatmap is bigger we need to recalculate
		glm::vec2 lfMin = logFile->getMin();
		glm::vec2 lfMax = logFile->getMax();

		if (lfMin.x < _currentMin.x ||
			lfMin.y < _currentMin.y ||
			lfMax.y > _currentMax.y ||
			lfMax.x > _currentMax.x)
		{
			_currentMin = logFile->getMin();
			_currentMax = logFile->getMax();

			_heatmaps.push_back(new Histogram(logFile, _binX, _binY));
			// update buffer
			GLint _currentVertexByteSize = _heatmaps.back()->getVertexDataBufferSize();
			allocateVertexBufferObject(_currentVertexByteSize, _heatmaps.back()->getVertexData());

			// update other heatmaps
			for (int i = 0; i < _heatmaps.size() - 1; i++) // miss last one
			{
				_heatmaps[i]->recalculate(lfMin, lfMax);
			}
		}
		else
		{
			_heatmaps.push_back(new Histogram(logFile, _binX, _binY, _heatmaps.back()->getMin(), _heatmaps.back()->getMax()));
		}

		_heatmaps.back()->setColours(_quadColours[_heatmaps.size() - 1]);
	}
}

void HistogramHandler::render()
{
	if (_isInit)
	{
		glBindVertexArray(_vertextArrayObject);

		int binIndex = 0;
		for (int i = 0; i < _heatmaps[0]->getVertexDataSize(); i += 6)
		{
			RGB color;

			if (_heatmaps.size() == 1)
				color = _heatmaps[0]->getBinColour(binIndex);
			else
			{
				// need to interpolate between the 2 colours
				color = _heatmaps[0]->getBinColour(binIndex);
				for (int j = 1; j < _heatmaps.size(); j++)
				{
					RGB hMapColour = _heatmaps[j]->getBinColour(binIndex);
					color = interpolate(color, hMapColour);
				}
			}

			glUniform3f(_colourLocation, color.r, color.g, color.b);
			glDrawArrays(GL_TRIANGLES, i, 12);
			binIndex++;
		}

		glBindVertexArray(0);
	}
}

void HistogramHandler::Initialise()
{
	if (!_isInit)
	{
		_isInit = true;

		// Vertex Buffer
		GLuint VDBO;

		glGenBuffers(1, &VDBO);

		_vertextBufferObject = VDBO;

		GLint _currentVertexByteSize = _heatmaps[0]->getVertexDataBufferSize();
		allocateVertexBufferObject(_currentVertexByteSize, _heatmaps[0]->getVertexData());

		std::cout << "vertexDataBufferObject created OK! GLUint is: " << VDBO << std::endl;


		// Vertex Array Object

		GLuint VAO; // hold the current value for the VAO

		glGenVertexArrays(1, &VAO); //create a Vertex Array Object

		glBindVertexArray(VAO); //make the just created vertexArrayObject the active one

		glBindBuffer(GL_ARRAY_BUFFER, VDBO); //bind vertexDataBufferObject

		glEnableVertexAttribArray(_positionLocation); //enable attribute at index positionLocation

		glVertexAttribPointer(_positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation

		glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

							  //cleanup
		glDisableVertexAttribArray(_positionLocation); //disable vertex attribute at index positionLocation
		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

		std::cout << "Vertex Array Object created OK! GLUint is: " << VAO << std::endl;

		_vertextArrayObject = VAO;
	}
}

void HistogramHandler::allocateVertexBufferObject(GLsizeiptr size, const GLvoid * data)
{
	glBindBuffer(GL_ARRAY_BUFFER, _vertextBufferObject);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RGB HistogramHandler::interpolate(RGB colour1, RGB colour2)
{
	GLfloat fraction = colour2.percent / (colour1.percent + colour2.percent);
	GLfloat r, g, b;
	// using algorithm from http://stackoverflow.com/questions/13488957/interpolate-from-one-color-to-another
	r = (colour2.r - colour1.r) * fraction + colour1.r;
	g = (colour2.g - colour1.g) * fraction + colour1.g;
	b = (colour2.b - colour1.b) * fraction + colour1.b;

	RGB returnVal(r, g, b);
	if (colour1.percent > colour2.percent)
		returnVal.percent = colour1.percent;
	else
		returnVal.percent = colour2.percent;
	return returnVal;
}
