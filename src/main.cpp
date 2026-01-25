// STL headers
#include <iostream>

// GLFW headers
#include "GLFW/glfw3.h"

// Project headers
#include "user.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto user = static_cast<User*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS)
	{
		user->pressedKeys.insert(key);
	}
	else if (action == GLFW_RELEASE)
	{
		user->pressedKeys.erase(key);
	}
}

int main(void)
{
	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello, GLFW World!", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwGetInputMode(window, GLFW_CURSOR);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}