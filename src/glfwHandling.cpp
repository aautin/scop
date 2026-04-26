#include "glfwHandling.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

void errorCallback(int code, const char* description)
{
    std::cout << "Error " << code << ": " << description << std::endl;
}

void compileShaderErrors(GLuint shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
	}
}

void compileShaderProgramErrors(GLuint shaderProgram)
{
	GLint hasCompiled;
	char infoLog[1024];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
		std::cout << "SHADER_LINKING_ERROR for shader program\n" << infoLog << std::endl;
	}
}
