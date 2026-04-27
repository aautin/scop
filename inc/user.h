#pragma once

// STL headers
#include <set>
#include <string>

// Project headers
#include "geometric.h"
#include "object.h"

struct SUser
{
	std::set<int> pressedKeys;
	CObjects      objects;
	SVertices     positionVertices;
	SVertices     textureVertices;
	SVertices     normalVertices;
	float         scale = 1.0f;
};
