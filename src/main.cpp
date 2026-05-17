#define STB_IMAGE_IMPLEMENTATION

// STL headers
#include <chrono>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <vector>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

// Project headers
#include "glfwHandling.h"
#include "user.h"
#include "objFile.h"
#include "stbImage.h"
#include "vertex.h"
#include "matrix.h"

static std::string getFileContent(const char* filename)
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

int main(int argc, char** argv)
{
	//-------------------------------//
	//- Parsing                     -//
	//-------------------------------//
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " [<file.obj> ...]" << std::endl;
		return EXIT_FAILURE;
	}

	SUser user;
	std::vector<CObjFile> parsedFiles;
	parsedFiles.reserve(argc - 1);
	for (int i = 1; i < argc; ++i)
	{
		try
		{
			parsedFiles.emplace_back(argv[i]);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error while parsing the file: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

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

	for (auto& objFile : parsedFiles)
	{
		objFile.addToUser(user);
	}

	glViewport(0, 0, 640, 480);

	glfwSetWindowUserPointer(window, &user);
	glfwSetKeyCallback(window, keyReleaseHandler);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);

	//-------------------------------//
	//- Vertex and fragment shaders -//
	//-------------------------------//
	const std::string vertexShaderSource = getFileContent("shaders/default.vert");
	const std::string fragmentShaderSource = getFileContent("shaders/default.frag");
	
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
	//- Vertices properties         -//
	//-------------------------------//
	fillVertices(user);
	placeObjectsSideBySide(user);
	
	//-------------------------------//
	//- Bindings                    -//
	//-------------------------------//
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//
	// We bind the vertex array object first because it store the buffers bindings
	//
	glBindVertexArray(VAO);

	//
	// We use a vertex buffer object to store vertices in GPU memory
	//
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, user.vertices.size() * sizeof(SVertex), user.vertices.data(), GL_STATIC_DRAW);
	
	//
	// Location 0 in source shader, 3 floats for color, structure declaration padding
	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, position));
	glEnableVertexAttribArray(0);

	//
	// Location 1 in source shader, 3 floats for color, structure declaration padding
	//
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, color));
	glEnableVertexAttribArray(1);

	//
	// Location 2 in source shader, 2 floats for texture, structure declaration padding
	//
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, texture));
	glEnableVertexAttribArray(2);

	//
	// Location 3 in source shader, 3 floats for normal, structure declaration padding
	//
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, normal));
	glEnableVertexAttribArray(3);

	//
	// Unbind to prevent accidental modifications
	// Note: EBO shouldnt be unbound while VAO still bound, because VAO stores EBO binding
	glBindBuffer(GL_ARRAY_BUFFER, 0);         // Unbind VBO
	glBindVertexArray(0);                     // Unbind VAO

	//-------------------------------//
	//- Textures                    -//
	//-------------------------------//
	//
	// Load the texture data and its properties
	// Reverse the image on y axis to match OpenGL texture coordinates
	//
	stbi_set_flip_vertically_on_load(true);
	
	int width, height, channels;
	unsigned char* textureData = stbi_load("assets/textures/plankoCat.png", &width, &height, &channels, 0);
	
	//
	// Generate one texture
	//
	GLuint texture;
	glGenTextures(1, &texture);

	//
	// Bind the texture to configure, and set its properties
	//
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	//
	// Set the texture mipmap (optimization for rendering textures far from the camera)
	//
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//
	// Unbind the texture for the good practice and free the texture data from CPU memory
	//
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(textureData);

	//-------------------------------//
	//- Uniforms                    -//
	//-------------------------------//
	//
	// We uniforms to have shared variables between CPU and GPU, they're global to the
	// shader program scope, updating a uniform can only be done between drawing calls
	//
	GLuint useTextureId = glGetUniformLocation(shaderProgram, "uUseTexture");
	
	GLuint KaId    = glGetUniformLocation(shaderProgram, "uKa");
	GLuint KdId    = glGetUniformLocation(shaderProgram, "uKd");
	GLuint KsId    = glGetUniformLocation(shaderProgram, "uKs");
	GLuint NsId    = glGetUniformLocation(shaderProgram, "uNs");
	GLuint NiId    = glGetUniformLocation(shaderProgram, "uNi");
	GLuint dId     = glGetUniformLocation(shaderProgram, "uD");
	GLuint illumId = glGetUniformLocation(shaderProgram, "uIllum");

	GLuint modelMatrixId      = glGetUniformLocation(shaderProgram, "uModelMatrix");
	GLuint viewMatrixId       = glGetUniformLocation(shaderProgram, "uViewMatrix");
	GLuint projectionMatrixId = glGetUniformLocation(shaderProgram, "uProjectionMatrix");

	GLuint lightPosId   = glGetUniformLocation(shaderProgram, "uLightPosition");
	GLuint lightColorId = glGetUniformLocation(shaderProgram, "uLightColor");

	GLuint cameraPositionId = glGetUniformLocation(shaderProgram, "uCameraPosition");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);

	//-------------------------------//
	//- Main loop (events and draw) -//
	//-------------------------------//
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(VAO);

		glUseProgram(shaderProgram);
		glUniform1i(useTextureId, user.useTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		auto viewMatrix = getViewMatrix(user);

		glUniform3f(lightPosId, user.lightPosition.x, user.lightPosition.y, user.lightPosition.z);
		glUniform3f(lightColorId, user.lightColor.r, user.lightColor.g, user.lightColor.b);
		glUniform3f(cameraPositionId, user.cameraPosition.x, user.cameraPosition.y, user.cameraPosition.z);
		glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, viewMatrix.data());
		glUniformMatrix4fv(projectionMatrixId, 1, GL_FALSE, user.projectionMatrix.data());

		for (const auto& [fileName, file] : user.files)
		{
			for (const auto& [name, object] : file.objects)
			{
				SMat4 modelMatrix = object.translation * object.rotation;
				glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, modelMatrix.data());

				for (const auto& [materialName, material] : file.materials)
				{
					if (!object.materialGroups.contains(materialName))
					{
						continue;
					}

					glUniform3f(KaId, material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
					glUniform3f(KdId, material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b);
					glUniform3f(KsId, material.specularColor.r, material.specularColor.g, material.specularColor.b);
					glUniform1f(NsId, material.specularExponent);
					glUniform1f(NiId, material.opticalDensity);
					glUniform1f(dId, material.dissolve);
					glUniform1i(illumId, material.illuminationModel);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.materialGroups.at(materialName).ebo);

					glDrawElements(GL_TRIANGLES, object.materialGroups.at(materialName).triangles.size() * 3, GL_UNSIGNED_INT, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				}
			}
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);

		glfwPollEvents();
		handlePressedKeys(window);
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