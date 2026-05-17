#pragma once

// STL headers
#include <set>
#include <string>

// Project headers
#include "math3d.h"
#include "types.h"
#include "objFile.h"

struct SUser
{
	SFilesMap    files;
	SVerticesVec vertices;

	bool  useTexture = false;
	SMat4* selectedRotationMatrix = nullptr;

	// Camera
	SVec3 cameraPosition = {0.0f, 0.0f, 3.0f};
	SVec3 cameraUp       = {0.0f, 1.0f, 0.0f};

	float cameraYaw   = -90.0f;
	float cameraPitch =  0.0f;

	// Light
	SColor lightColor    = {1.0f, 1.0f, 1.0f};
	SVec3  lightPosition = {0.0f, 0.0f, 0.0f};

	// Matrices
	SMat4 projectionMatrix = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
};
