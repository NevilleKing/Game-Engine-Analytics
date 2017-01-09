// tag::C++11check[]
#define STRING2(x) #x
#define STRING(x) STRING2(x)

#if __cplusplus < 201103L
#pragma message("WARNING: the compiler may not be C++11 compliant. __cplusplus version is : " STRING(__cplusplus))
#endif
// end::C++11check[]

// tag::includes[]
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include <GL/glew.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// custom classes
#include "LogFile.h"

// end::includes[]

// tag::using[]
// see https://isocpp.org/wiki/faq/Coding-standards#using-namespace-std
// don't use the whole namespace, either use the specific ones you want, or just type std::
using std::cout;
using std::endl;
using std::max;
// end::using[]


// tag::globalVariables[]
std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext
int frameCount = 0;
std::string frameLine = "";

std::vector<LogFile*> logFiles;
// end::globalVariables[]

// tag::vertexShader[]
//string holding the **source** of our vertex shader, to save loading from a file
const std::string strVertexShader = R"(
	#version 330
	in vec2 position;

	uniform mat4 modelMatrix      = mat4(1.0);
	uniform mat4 viewMatrix       = mat4(1.0);
	uniform mat4 projectionMatrix = mat4(1.0);
	void main()
	{
		 gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 0.0, 1.0);
	}
)";
// end::vertexShader[]

// tag::fragmentShader[]
//string holding the **source** of our fragment shader, to save loading from a file
const std::string strFragmentShader = R"(
	#version 330
	out vec4 outputColor;
	uniform vec3 color;
	void main()
	{
		 outputColor = vec4(color, 1.0f);
	}
)";
// end::fragmentShader[]

// tag::ourVariables[]
//our variables
bool done = false;

//the color we'll pass to the GLSL
GLfloat color[] = { 1.0f, 1.0f, 1.0f }; //using different values from CPU and static GLSL examples, to make it clear this is working

										//our GL and GLSL variables

GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)
GLint positionLocation; //GLuint that we'll fill in with the location of the `position` attribute in the GLSL
GLint colorLocation; //GLuint that we'll fill in with the location of the `color` variable in the GLSL

GLint projectionMatrixLocation; // location of the projection matrix in the GLSL

GLuint vertexDataBufferObjects[1];
GLuint vertexArrayObjects[1];

GLfloat lineWidth = 1.0f;

// for dragging stuff
bool mouseDown = false;
glm::vec2 offsetVector = glm::vec2(0.0f, 0.0f);

// scale of map (for zooming)
GLfloat zoomScale = 1.0f;

// end::ourVariables[]


// end Global Variables
/////////////////////////

// tag::initialise[]
void initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	cout << "SDL initialised OK!\n";
}
// end::initialise[]

// tag::createWindow[]
void createWindow()
{
	//get executable name, and use as window title
	int beginIdxWindows = exeName.rfind("\\"); //find last occurrence of a backslash
	int beginIdxLinux = exeName.rfind("/"); //find last occurrence of a forward slash
	int beginIdx = max(beginIdxWindows, beginIdxLinux);
	std::string exeNameEnd = exeName.substr(beginIdx + 1);
	const char *exeNameCStr = exeNameEnd.c_str();

	//create window
	win = SDL_CreateWindow(exeNameCStr, 100, 100, 1000, 600, SDL_WINDOW_OPENGL); //same height and width makes the window square ...

																				//error handling
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "SDL CreatedWindow OK!\n";
}
// end::createWindow[]

// tag::setGLAttributes[]
void setGLAttributes()
{
	int major = 3;
	int minor = 3;
	cout << "Built for OpenGL Version " << major << "." << minor << endl; //ahttps://en.wikipedia.org/wiki/OpenGL_Shading_Language#Versions
																		  // set the opengl context version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //core profile
	cout << "Set OpenGL context to versicreate remote branchon " << major << "." << minor << " OK!\n";
}
// tag::setGLAttributes[]

// tag::createContext[]
void createContext()
{
	setGLAttributes();

	context = SDL_GL_CreateContext(win);
	if (context == nullptr) {
		SDL_DestroyWindow(win);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}
// end::createContext[]

// tag::initGlew[]
void initGlew()
{
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev) {
		std::cout << "GLEW Error: " << glewGetErrorString(rev) << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLEW Init OK!\n";
	}
}
// end::initGlew[]

// tag::createShader[]
GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	//error check
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}
// end::createShader[]

// tag::createProgram[]
GLuint createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}
// end::createProgram[]

// tag::initializeProgram[]
void initializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(createShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = createProgram(shaderList);
	if (theProgram == 0)
	{
		cout << "GLSL program creation error." << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLSL program creation OK! GLUint is: " << theProgram << std::endl;
	}

	positionLocation = glGetAttribLocation(theProgram, "position");
	projectionMatrixLocation = glGetUniformLocation(theProgram, "projectionMatrix");
	colorLocation = glGetUniformLocation(theProgram, "color");
	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}
// end::initializeProgram[]

// tag::initializeVertexArrayObject[]
//setup a GL object (a VertexArrayObject) that stores how to access data and from where
void initializeVertexArrayObject()
{
	glGenVertexArrays(logFiles.size(), vertexArrayObjects); //create a Vertex Array Object

	// loop through and create all the vetex objects
	for (int i = 0; i < logFiles.size(); i++)
	{
		glBindVertexArray(vertexArrayObjects[i]); //make the just created vertexArrayObject the active one

		glBindBuffer(GL_ARRAY_BUFFER, vertexDataBufferObjects[i]); //bind vertexDataBufferObject

		glEnableVertexAttribArray(positionLocation); //enable attribute at index positionLocation

		glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0); //specify that position data contains four floats per vertex, and goes into attribute index positionLocation

		glBindVertexArray(0); //unbind the vertexArrayObject so we can't change it

		//cleanup
		glDisableVertexAttribArray(positionLocation); //disable vertex attribute at index positionLocation
		glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind array buffer

		cout << "Vertex Array Object created OK! GLUint is: " << vertexArrayObjects[i] << std::endl;
	}
}
// end::initializeVertexArrayObject[]

// tag::initializeVertexBuffer[]
void initializeVertexBuffer()
{
	glGenBuffers(logFiles.size(), vertexDataBufferObjects);

	// loop through and create all the buffer objects
	for (int i = 0; i < logFiles.size(); i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataBufferObjects[i]);
		glBufferData(GL_ARRAY_BUFFER, logFiles[i]->getData().size() * sizeof(logFiles[i]->getData()), &logFiles[i]->getData()[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		cout << "vertexDataBufferObject created OK! GLUint is: " << vertexDataBufferObjects[i] << std::endl;
	}
	
	initializeVertexArrayObject();
}
// end::initializeVertexBuffer[]

// tag::loadAssets[]
void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program, and get IDs of attributes and variables

	initializeVertexBuffer(); //load data into a vertex buffer

	cout << "Loaded Assets OK!\n";
}
// end::loadAssets[]

// tag::handleInput[]
void handleDropEvent(char* filePath) 
{
	// When the user drops a file onto the window, this function is called
	std::cout << "File dropped: " << filePath << std::endl;
}

void zoomIn()
{
	zoomScale += 0.1f;
}

void zoomOut()
{
	if ((zoomScale - 0.1f) > 0.0f)
	{
		zoomScale -= 0.1f;
	}
}

void handleInput()
{
	//Event-based input handling
	//The underlying OS is event-based, so **each** key-up or key-down (for example)
	//generates an event.
	//  - https://wiki.libsdl.org/SDL_PollEvent
	//In some scenarios we want to catch **ALL** the events, not just to present state
	//  - for instance, if taking keyboard input the user might key-down two keys during a frame
	//    - we want to catch based, and know the order
	//  - or the user might key-down and key-up the same within a frame, and we still want something to happen (e.g. jump)
	//  - the alternative is to Poll the current state with SDL_GetKeyboardState

	SDL_Event event; //somewhere to store an event

					 //NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) //loop until SDL_PollEvent returns 0 (meaning no more events)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set donecreate remote branch flag if SDL wants to quit (i.e. if the OS has triggered a close event,
						 //  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			//Keydown can fire repeatable if key-repeat is on.
			//  - the repeat flag is set on the keyboard event, if this is a repeat event
			//  - in our case, we're going to ignore repeat events
			//  - https://wiki.libsdl.org/SDL_KeyboardEvent
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
				{
					//hit escape to exit
				case SDLK_ESCAPE: 
					done = true;
					break;
				case SDLK_LEFTBRACKET:
					if (lineWidth > 1.0f) // min width
						lineWidth -= 1.f;
					break;
				case SDLK_RIGHTBRACKET: 
					if (lineWidth < 7.0f) // max width
						lineWidth += 1.f;
					break;
				case SDLK_MINUS:
					zoomOut();
					break;
				case SDLK_EQUALS:
					zoomIn();
					break;
				}
			break;
			// Dropped file handling
		case SDL_DROPFILE:
		{
			char* drop_file_dir = event.drop.file;
			handleDropEvent(drop_file_dir);
			SDL_free(drop_file_dir);    // Free dropped_filedir memory
		}
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseDown = true;
			if (event.button.clicks == 2)
				zoomIn();
			break;
		case SDL_MOUSEBUTTONUP:
			mouseDown = false;
			break;
		case SDL_MOUSEMOTION:
			if (mouseDown)
				offsetVector += glm::vec2(event.motion.xrel, -event.motion.yrel) * 2.0f;
			break;
		}
	}
}
// end::handleInput[]

// tag::updateSimulation[]
void updateSimulation(double simLength = 0.02) //update simulation with an amount of time to simulate for (in seconds)
{
	//WARNING - we should calculate an appropriate amount of time to simulate - not always use a constant amount of time
	// see, for example, http://headerphile.blogspot.co.uk/2014/07/part-9-no-more-delays.html

	//CHANGE ME
}
// end::updateSimulation[]

// tag::preRender[]
void preRender()
{
	glViewport(0, 0, 1000, 600); //set viewpoint
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f); //set clear colour
	glClear(GL_COLOR_BUFFER_BIT); //clear the window (technical the scissor box bounds)
}
// end::preRender[]

// tag::render[]
void render()
{
	glUseProgram(theProgram); //installs the program object specified by program as part of current rendering state

							  //load data to GLSL that **may** have changed
	glUniform3f(colorLocation, color[0], color[1], color[2]);
	//alternatively, use glUnivform2fv
	//glUniform2fv(colorLocation, 1, color); //Note: the count is 1, because we are setting a single uniform vec2 - https://www.opengl.org/wiki/GLSL_:_common_mistakes#How_to_use_glUniform

	glm::mat4 projectionMatrix = glm::ortho(-1000.0f, 4000.0f, -1700.0f, 2300.0f);
	// scale the map (zoom)
	projectionMatrix = glm::scale(projectionMatrix, glm::vec3(zoomScale, zoomScale, 0.0f));
	// move the map to the offset location (where the user has moved it to)
	projectionMatrix = glm::translate(projectionMatrix, glm::vec3(offsetVector, 0.0f));
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// set the line width based on our variable
	glLineWidth(lineWidth);

	for (int i = 0; i < logFiles.size(); i++)
	{
		glBindVertexArray(vertexArrayObjects[i]);

		for (int j = 0; j < logFiles[i]->getData().size() - 1; j++)
		{
			glDrawArrays(GL_LINES, j, 2); //Draw something, using Triangles, and 3 vertices - i.e. one lonely triangle
		}

		glBindVertexArray(0);
	}

	glUseProgram(0); //clean up

}
// end::render[]

// tag::postRender[]
void postRender()
{
	SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)
	frameLine += "Frame: " + std::to_string(frameCount++);
	cout << "\r" << frameLine << std::flush;
	frameLine = "";
}
// end::postRender[]

// tag::cleanUp[]
void cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}
// end::cleanUp[]

// tag::main[]
int main(int argc, char* args[])
{
	exeName = args[0];

	// loop through arguments loading in the log files
	for (int i = 1; i < argc; i++)
	{
		std::string logPath = args[1];

		LogFile* lf = new LogFile(logPath);

		if (lf->getStatus() == LogFile::STATUS_ERROR)
		{
			// handle error
			std::cout << "Error parsing log file" << std::endl;
		}
		else
		{
			logFiles.push_back(lf);
		}
	}

	//setup
	//- do just once
	initialise();
	createWindow();

	createContext();

	initGlew();

	glViewport(0, 0, 1000, 600); //should check what the actual window res is?

	SDL_GL_SwapWindow(win); //force a swap, to make the trace clearer

							//do stuff that only needs to happen once
							//- create shaders
							//- load vertex data
	loadAssets();

	while (!done) //loop until done flag is set)
	{
		handleInput(); // this should ONLY SET VARIABLES

		updateSimulation(); // this should ONLY SET VARIABLES according to simulation

		preRender();

		render(); // this should render the world state according to VARIABLES -

		postRender();

	}

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}
// end::main[]