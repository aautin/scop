#pragma once

// STL headers
#include <vector>
#include <string>

// Project headers
#include "geometric.h"

struct SObject
{
	std::string            name;
	std::vector<STriangle> triangles;
};
using SObjects = std::vector<SObject>;
