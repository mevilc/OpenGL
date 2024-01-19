#include <glad/glad.h> // contains function pointers to OpenGL functions
#include <SDL2/SDL.h>
#undef main
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

const int screenHeight = 480;
const int screenWidth = 600;

SDL_Window* window = nullptr;
SDL_GLContext openGLContext = nullptr;

bool quit = false;

// IDs for VAO and VBO
GLuint vertexArrayObject = 0;
GLuint vertexBufferObject = 0;

// Program object (ID for graphics pipeline)
GLuint graphicsPipelineShaderProgram = 0;



static std::string loadShaderAsString(const std::string& filename)
{
	std::string result = ""; // to store shader program
	std::string line;
	std::ifstream file(filename);

	if (file.is_open())
	{
		while (std::getline(file, line))
			result += line + "\n";

		file.close();
	}
	return result;
}

static GLuint compileShader(GLuint type, const std::string source)
{
	GLuint shaderObject{ };

	if (type == GL_VERTEX_SHADER)
	{
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == GL_FRAGMENT_SHADER)
	{
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	return shaderObject;
}

static GLuint createShaderProgram(const std::string& vertexShaderSrc, 
	const std::string& fragmentShaderSrc)
{
	// create a new program object to hold vs and fs objects
	GLuint programObject = glCreateProgram();

	// compiles the shader src's
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

	// attach shaders to program object
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	glLinkProgram(programObject);

	// validate program
	glValidateProgram(programObject);

	// detach shaders
	glDetachShader(programObject, vertexShader);
	glDetachShader(programObject, fragmentShader);

	return programObject;
}

static void createGraphicsPipeline()
{
	// can load shader source from file
	std::string vertexShaderSrc = loadShaderAsString(R"(C:\OpenGL\Shaders\vert.glsl.txt)");
	std::string fragmentShaderSrc = loadShaderAsString(R"(C:\OpenGL\Shaders\frag.glsl.txt)");
	graphicsPipelineShaderProgram = createShaderProgram(vertexShaderSrc, 
		fragmentShaderSrc);
}

// samity check
static void getOpenGLVersionInfo()
{
	std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
	std::cout << "OpenGL shading langauge version: " 
		<< glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

static void vertexSpecification()
{
	// on CPU
	const std::vector<GLfloat> vertexPos = {
		// OpenGL coords : [-1, 1]
		// triangle 1
		// X      Y     Z
		-0.8f, -0.8f, 0.0f,	// vertex 1
		-0.2f, -0.8f, 0.0f, // vertex 2
		-0.4f,  0.0f, 0.0f, // vertex 3

		 0.5f, 0.0f, 0.0f,	// vertex 1
		 0.8f, 0.0f, 0.0f,  // vertex 2
		 0.0f, 0.8f, 0.0f   // vertex 3
	};

	// ship to GPU
	// set up VAO with an ID (vertexArrayObject)
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject); // use this VAO

	// set up VBO
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // use this VBO
	// loads data to VBO
	glBufferData(GL_ARRAY_BUFFER,
		vertexPos.size() * sizeof(GLfloat),
		vertexPos.data(),
		GL_STATIC_DRAW);

	// telling how VAO should access VBO
	// enables attribute (position, here)
	glEnableVertexAttribArray(0);
	// x, y, z (3 fp elements/vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// bind with 0, aka done with VAO
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
}

static void initialize()
{	
	// initialize SDL with VIDEO subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Failed to initialize SDL2" << "\n";
		exit(1);
	}

	// set some attributes of the context
	// OpenGL 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// use OpenGL core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// bit depth = 24 bits
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create a window to interact with OpenGL
	window = SDL_CreateWindow("window", 50, 50, screenWidth, screenHeight,
		SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cerr << "Failed to create SDL2 window" << "\n";
		exit(1);
	};
	
	// create an OpenGL context to be contained in window
	openGLContext = SDL_GL_CreateContext(window);
	if (openGLContext == nullptr)
	{
		std::cerr << "Failed to create OpenGL context" << "\n";
		exit(1);
	}
	
	// initialize GLAD
	// loading OpenGL function pointers 
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << "\n";
		exit(1);
	}

	getOpenGLVersionInfo();
}

static void input()
{
	SDL_Event e;
	// keep polling for events
	while (SDL_PollEvent(&e) != 0)
	{
		// there is some event 
		if (e.type == SDL_QUIT)
		{
			// event = to quit
			std::cout << "Bye!" << "\n";
			quit = true;
		}
	}
}

static void preDraw()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	// part of window used for rendering
	glViewport(0, 0, screenWidth, screenHeight);
	// bkg color of scene
	glClearColor(1.f, 1.f, 0.f, 1.f);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// use this pipeline
	glUseProgram(graphicsPipelineShaderProgram);
}

static void draw()
{
	// use this VAO
	glBindVertexArray(vertexArrayObject);
	// use this VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void mainLoop()
{
	// main app loop
	while (!quit)
	{
		// handles input
		input();

		preDraw();

		draw();

		// updates the window every frame
		// double buffering: draw to the back (back buffer) and when its done
		// push it to the front (front buffer) to display
		// makes transitions smooth
		SDL_GL_SwapWindow(window);
	}
}

static void cleanup()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main()
{
	// set up SDL, OpenGL context, GLAD
	initialize();

	// set up vertices, VAO, VBO 
	vertexSpecification();

	// creating graphics pipeline with vertex and fragment shader on geometry
	createGraphicsPipeline();

	mainLoop();

	cleanup();

	return 0;
}