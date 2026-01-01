#include "GLFW/glfw3.h"
#include <stdio.h>

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		printf("click\n");
}

int main(void)
{
	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello, GLFW World!", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwGetInputMode(window, GLFW_CURSOR);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}