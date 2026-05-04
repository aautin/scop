#include "glfwHandling.h"

// STL headers
#include <algorithm>
#include <functional>

void keyReleaseHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));

	const std::map<int, std::function<void()>> keyBindings = {
		{GLFW_KEY_M, [user](){ user->useTexture = !user->useTexture; }},
	};

	if (action == GLFW_RELEASE)
	{
		for (const auto& binding : keyBindings)
		{
			if (binding.first == key)
			{
				binding.second();
				break;
			}
		}
	}
}

void handlePressedKeys(GLFWwindow* window)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));

	const std::map<EPressInputAction, std::set<int>> keyBindings = {
		{MOVE_VIEW_BACKWARD, {GLFW_KEY_S}},
		{MOVE_VIEW_FORWARD,  {GLFW_KEY_W}},
		{MOVE_VIEW_LEFT,     {GLFW_KEY_A}},
		{MOVE_VIEW_RIGHT,    {GLFW_KEY_D}},

		{MOVE_LIGHT_BACKWARD, {GLFW_KEY_KP_2}},
		{MOVE_LIGHT_FORWARD,  {GLFW_KEY_KP_8}},
		{MOVE_LIGHT_LEFT,     {GLFW_KEY_KP_4}},
		{MOVE_LIGHT_RIGHT,    {GLFW_KEY_KP_6}},
		
		{ROTATE_MODEL_X_POS, {GLFW_KEY_X, GLFW_KEY_UP}},
		{ROTATE_MODEL_X_NEG, {GLFW_KEY_X, GLFW_KEY_DOWN}},
		{ROTATE_MODEL_Y_POS, {GLFW_KEY_Y, GLFW_KEY_UP}},
		{ROTATE_MODEL_Y_NEG, {GLFW_KEY_Y, GLFW_KEY_DOWN}},
		{ROTATE_MODEL_Z_POS, {GLFW_KEY_Z, GLFW_KEY_UP}},
		{ROTATE_MODEL_Z_NEG, {GLFW_KEY_Z, GLFW_KEY_DOWN}},
		
		{CLOSE_APPLICATION,  {GLFW_KEY_ESCAPE}},
	};

	for (const auto& [action, keys] : keyBindings)
	{
		if (std::all_of(keys.begin(), keys.end(), [&](int key)
		{ return glfwGetKey(window, key) == GLFW_PRESS; }))
		{
			switch (action)
			{
				case MOVE_VIEW_BACKWARD:
				{
					user->viewMatrix = glm::translate(user->viewMatrix, glm::vec3(0.0f, 0.0f, -0.1f));
					break;
				}
				case MOVE_VIEW_FORWARD:
				{
					user->viewMatrix = glm::translate(user->viewMatrix, glm::vec3(0.0f, 0.0f, 0.1f));
					break;
				}
				case MOVE_VIEW_LEFT:
				{
					user->viewMatrix = glm::translate(user->viewMatrix, glm::vec3(0.1f, 0.0f, 0.0f));
					break;
				}
				case MOVE_VIEW_RIGHT:
				{
					user->viewMatrix = glm::translate(user->viewMatrix, glm::vec3(-0.1f, 0.0f, 0.0f));
					break;
				}
				case ROTATE_MODEL_X_POS:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(1.0f, 0.0f, 0.0f));
					break;
				}
				case ROTATE_MODEL_X_NEG:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(-1.0f, 0.0f, 0.0f));
					break;
				}
				case ROTATE_MODEL_Y_POS:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(0.0f, 1.0f, 0.0f));
					break;
				}
				case ROTATE_MODEL_Y_NEG:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(0.0f, -1.0f, 0.0f));
					break;
				}
				case ROTATE_MODEL_Z_POS:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(0.0f, 0.0f, 1.0f));
					break;
				}
				case ROTATE_MODEL_Z_NEG:
				{
					user->modelMatrix = glm::rotate(user->modelMatrix, glm::radians(5.0f),
																	   glm::vec3(0.0f, 0.0f, -1.0f));
					break;
				}
				case MOVE_LIGHT_BACKWARD:
				{
					user->lightPosition.z -= 0.1f;
					break;
				}
				case MOVE_LIGHT_FORWARD:
				{
					user->lightPosition.z += 0.1f;
					break;
				}
				case MOVE_LIGHT_LEFT:
				{
					user->lightPosition.x -= 0.1f;
					break;
				}
				case MOVE_LIGHT_RIGHT:
				{
					user->lightPosition.x += 0.1f;
					break;
				}
				case CLOSE_APPLICATION:
				{
					glfwSetWindowShouldClose(window, true);
					break;
				}
			}
		}
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
