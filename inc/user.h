#pragma once

// STL headers
#include <set>
#include <string>

// Project headers
#include "types.h"

struct SUser
{
	SObjectsMap   objects;
	SVerticesVec  vertices;
	SMaterialsMap materials;

	bool  useTexture = false;

	glm::vec3 lightColor    = glm::vec3(1, 1, 1);
	glm::vec3 lightPosition = glm::vec3(0, 0, 0);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix  = glm::mat4(1.0f);
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
};
