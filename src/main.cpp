// STL headers
#include <iostream>
#include <chrono>

// Project headers
#include "user.h"
#include "file.h"

#define VERTICE_FLOAT_COUNT 6

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); ourColor = aColor; }\n\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main() { FragColor = vec4(ourColor, 1.0f); }\n\0";

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS)
	{
		user->pressedKeys.insert(key);
	}
	else if (action == GLFW_RELEASE)
	{
		user->pressedKeys.erase(key);
	}
	
}

static void error_callback(int code, const char* description)
{
    std::cout << "Error " << code << ": " << description << std::endl;
}

static void loadFile(SUser& user, const std::string& filename)
{
	CFile file(filename);
	user.objects = file.getObjects();
	user.vertices = file.getVertices();
}

static void handleKeys(const SUser& user, GLFWwindow* window)
{
	// std::cout << "----" << std::endl;
	// for (const auto& key : user.pressedKeys)
	// {
	// 	std::cout << "Key " << key << " is pressed" << std::endl;
	// }

	if (user.pressedKeys.contains(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <file.obj>" << std::endl;
		return EXIT_FAILURE;
	}

	glfwSetErrorCallback(error_callback);

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

	SUser user;
	loadFile(user, argv[1]);

	glfwSetWindowUserPointer(window, &user);
	glfwSetKeyCallback(window, keyCallback);

	//
	// Define the vertex and fragment shaders
	//
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	size_t verticesCount = 0;
	for (const auto& object : user.objects)
	{
		verticesCount += object.getTriangles().size();
	}
	verticesCount = verticesCount * 3;

	GLfloat* vertices = new GLfloat[verticesCount * VERTICE_FLOAT_COUNT]; // 3 for position and 3 for color
	size_t vertexIndex = 0;
	for (const auto& object : user.objects)
	{
		for (const auto& triangle : object.getTriangles())
		{
			auto chrono = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			for (size_t i = 0; i < 3; ++i)
			{
				auto vertex = user.vertices[triangle.verticesIndices[i] - 1];
				vertices[vertexIndex++] = vertex.x;
				vertices[vertexIndex++] = vertex.y;
				vertices[vertexIndex++] = vertex.z;
				
				vertices[vertexIndex++] = std::abs(std::sin(chrono));
				vertices[vertexIndex++] = std::abs(std::cos(chrono));
				vertices[vertexIndex++] = std::abs(std::tan(chrono));
			}
		}
	}

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesCount * VERTICE_FLOAT_COUNT * sizeof(float), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTICE_FLOAT_COUNT * sizeof(float), NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTICE_FLOAT_COUNT * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);
		glfwSwapBuffers(window);

		glfwPollEvents();
		handleKeys(user, window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}