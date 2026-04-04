#pragma once

// STL headers
#include <vector>

struct SVertex
{
	float x, y, z;
};
using SVertexes = std::vector<SVertex>;

struct SFace
{
	unsigned int vertexIndices[3];
};
using SFaces = std::vector<SFace>;

struct STriangle
{
	SVertex v1, v2, v3;
};
using STriangles = std::vector<STriangle>;
