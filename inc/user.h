#pragma once

// STL headers
#include <set>
#include <string>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

// Project headers
#include "geometric.h"
#include "object.h"

struct SUser
{
	std::set<int> pressedKeys;
	CObjects      objects;
	SVertices     vertices;
};
