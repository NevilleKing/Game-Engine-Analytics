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
	}
	else
	{
		// assume (for now) that it is smaller
		_heatmaps.push_back(new Histogram(logFile, _binX, _binY, _heatmaps.back()->getMin(), _heatmaps.back()->getMax()));
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
			GLfloat* color = _heatmaps[0]->getBinColour(binIndex);
			glUniform3f(_colourLocation, color[0], color[1], color[2]);
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
