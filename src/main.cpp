// STL headers
#include <chrono>
#include <iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<cerrno>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

// Project headers
#include "glfwHandling.h"
#include "user.h"
#include "objFile.h"

#define VERTICE_FLOAT_COUNT 6 // 3 for position and 3 for color

static std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}


template <typename T>
struct SPtr
{
	size_t size;
	T*     data;
};

static void loadFile(SUser& user, const std::string& filename)
{
	CObjFile file(filename);
	user.objects = file.getObjects();
	user.vertices = file.getVertices();
}

static void handleKeys(SUser& user, GLFWwindow* window)
{
	if (user.pressedKeys.contains(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (user.pressedKeys.contains(GLFW_KEY_UP))
	{
		std::cout << "UP key is pressed" << std::endl;
		user.scale += 0.01f;
	}
	else if (user.pressedKeys.contains(GLFW_KEY_DOWN))
	{
		std::cout << "DOWN key is pressed" << std::endl;
		user.scale -= 0.01f;
	}
}

int main(int argc, char** argv)
{
	//-------------------------------//
	//- Parsing                     -//
	//-------------------------------//
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <file.obj>" << std::endl;
		return EXIT_FAILURE;
	}

	SUser user;
	loadFile(user, argv[1]);

	//-------------------------------//
	//- OpenGL initialization       -//
	//-------------------------------//
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	//
	// We are using OpenGL 3.3 and the core profile to only have access to modern features
	//
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(640, 480, "Scop", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return EXIT_FAILURE;
	}

	glViewport(0, 0, 640, 480);

	
	glfwSetWindowUserPointer(window, &user);
	glfwSetKeyCallback(window, keyCallback);

	//-------------------------------//
	//- Vertex and fragment shaders -//
	//-------------------------------//
	const std::string vertexShaderSource = get_file_contents("shaders/default.vert");
	const std::string fragmentShaderSource = get_file_contents("shaders/default.frag");
	
	const char* vertexShaderSourcePtr = vertexShaderSource.c_str();
	const char* fragmentShaderSourcePtr = fragmentShaderSource.c_str();
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, NULL);
	glCompileShader(vertexShader);
	compileShaderErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, NULL);
	glCompileShader(fragmentShader);
	compileShaderErrors(fragmentShader, "FRAGMENT");

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	compileShaderProgramErrors(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//-------------------------------//
	//- Vertices and indices buffers-//
	//-------------------------------//
	SPtr<GLfloat> vertices =
	{
		(user.vertices.size() * VERTICE_FLOAT_COUNT) * sizeof(GLfloat),
		new GLfloat[user.vertices.size() * VERTICE_FLOAT_COUNT],
	};

	size_t indicesCount = 0;
	for (auto& object : user.objects) { indicesCount += object.getTriangles().size() * 3; }
	SPtr<GLuint> indices =
	{
		indicesCount * sizeof(GLuint),
		new GLuint[indicesCount],
	};

	{
		size_t index = 0;
		for (const auto& vertex : user.vertices)
		{
			auto chrono = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			vertices.data[index++] = vertex.x;
			vertices.data[index++] = vertex.y;
			vertices.data[index++] = vertex.z;
			
			vertices.data[index++] = std::abs(std::sin(chrono));
			vertices.data[index++] = std::abs(std::cos(chrono));
			vertices.data[index++] = std::abs(std::tan(chrono));
		}
	}

	{
		size_t index = 0;
		for (const auto& object : user.objects)
		{
			for (const auto& triangle : object.getTriangles())
			{
				indices.data[index++] = triangle.verticesIndices[0] - 1;
				indices.data[index++] = triangle.verticesIndices[1] - 1;
				indices.data[index++] = triangle.verticesIndices[2] - 1;
			}
		}
	}

	//-------------------------------//
	//- Bindings                    -//
	//-------------------------------//
	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//
	// We use a vertex buffer object to store vertices in GPU memory
	//
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size, vertices.data, GL_STATIC_DRAW);
	
	//
	// Location 0 in source shader, 3 floats for color, no padding
	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTICE_FLOAT_COUNT * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);

	//
	// Location 1 in source shader, 3 floats for color, 3-floats padded (position)
	//
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTICE_FLOAT_COUNT * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//
	// We use an element buffer to reuse vertices for multiple triangles
	//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size, indices.data, GL_STATIC_DRAW);

	//
	// Unbind to prevent accidental modifications
	// Note: EBO shouldnt be unbound while VAO still bound, because VAO stores EBO binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);         // Unbind VBO
	glBindVertexArray(0);                     // Unbind VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO

	//-------------------------------//
	//- Uniforms                    -//
	//-------------------------------//
	//
	// We uniforms to have shared variables between CPU and GPU, they're global to the
	// shader program scope, updating a uniform can only be done between drawing calls
	//
	GLuint scaleId = glGetUniformLocation(shaderProgram, "scale");

	//-------------------------------//
	//- Main loop (events and draw) -//
	//-------------------------------//
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1f(scaleId, user.scale);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);

		glfwPollEvents();
		handleKeys(user, window);
	}

	//-------------------------------//
	//- Cleanup                     -//
	//-------------------------------//
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}