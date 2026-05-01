#include "glfwHandling.h"

void keyReleaseHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_M:
				user->useTexture = !user->useTexture;
				break;
			default:
				break;
		}
	}
}

void handlePressedKeys(GLFWwindow* window)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		user->scale *= 1.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		user->scale /= 1.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void errorCallback(int code, const char* description)
{
    std::cerr << "Error " << code << ": " << description << std::endl;
}

void compileShaderErrors(GLuint shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cerr << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
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
		std::cerr << "SHADER_LINKING_ERROR for shader program\n" << infoLog << std::endl;
	}
}
