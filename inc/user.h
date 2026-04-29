#pragma once

// STL headers
#include <set>
#include <string>

// Project headers
#include "types.h"
#include "object.h"

struct SUser
{
	std::set<int> pressedKeys;
	CObjects      objects;
	float         scale = 1.0f;
};
