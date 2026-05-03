#pragma once

// STL headers
#include <iostream>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

#include "user.h"

enum EInputAction
{
	MOVE_VIEW_DOWN,
	MOVE_VIEW_UP,
	MOVE_VIEW_LEFT,
	MOVE_VIEW_RIGHT,
	
	ROTATE_MODEL_X_POS,
	ROTATE_MODEL_X_NEG,
	ROTATE_MODEL_Y_POS,
	ROTATE_MODEL_Y_NEG,
	ROTATE_MODEL_Z_POS,
	ROTATE_MODEL_Z_NEG,

	SWITCH_VIEW_MODE,
	CLOSE_APPLICATION,
};

void keyReleaseHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void handlePressedKeys(GLFWwindow* window);
void errorCallback(int code, const char* description);
void compileShaderErrors(GLuint shader, const char* type);
void compileShaderProgramErrors(GLuint shaderProgram);