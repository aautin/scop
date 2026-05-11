#pragma once

// STL headers
#include <iostream>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

#include "user.h"

typedef std::set<int> Keys; 

void keyReleaseHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
void handlePressedKeys(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void errorCallback(int code, const char* description);
void compileShaderErrors(GLuint shader, const char* type);
void compileShaderProgramErrors(GLuint shaderProgram);