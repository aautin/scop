// STL headers
#include <iostream>
#include <chrono>

// Project headers
#include "user.h"
#include "file.h"

#define VERTICE_FLOAT_COUNT 6 // 3 for position and 3 for color

template <typename T>
struct SPtr
{
	size_t size;
	T*     data;
};

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
	if (user.pressedKeys.contains(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
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

	
	glfwSetWindowUserPointer(window, &user);
	glfwSetKeyCallback(window, keyCallback);

	//-------------------------------//
	//- Vertex and fragment shaders -//
	//-------------------------------//
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
	//- Main loop (events and draw) -//
	//-------------------------------//
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);

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