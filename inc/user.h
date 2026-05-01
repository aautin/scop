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

	float scale = 1.0f;
	bool  useTexture = false;
};
