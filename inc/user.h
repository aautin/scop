#pragma once

// STL headers
#include <set>
#include <string>

// Project headers
#include "types.h"

struct SUser
{
	// Objects, vertices and materials
	SObjectsMap   objects;
	SVerticesVec  vertices;
	SMaterialsMap materials;

	bool  useTexture = false;

	// Camera
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraUp       = glm::vec3(0.0f, 1.0f, 0.0f);

	float cameraYaw = -90.0f;
	float cameraPitch = 0.0f;

	// Light
	glm::vec3 lightColor    = glm::vec3(1, 1, 1);
	glm::vec3 lightPosition = glm::vec3(0, 0, 0);

	// Matrices
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
};
