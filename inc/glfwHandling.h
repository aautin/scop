#pragma once

// STL headers
#include <iostream>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

#include "user.h"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void errorCallback(int code, const char* description);
void compileShaderErrors(GLuint shader, const char* type);
void compileShaderProgramErrors(GLuint shaderProgram);