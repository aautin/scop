#pragma once


// STL headers
#include <vector>
#include <limits>
#include <cmath>

enum ESquareType
{
	Concave,
	Convex,
};

struct SVertex
{
	float x, y, z;
};
using SVertices = std::vector<SVertex>;

struct SFace
{
	std::vector<size_t> verticesIndices;
};
using SFaces = std::vector<SFace>;

struct STriangle
{
	size_t verticesIndices[3];
};
using STriangles = std::vector<STriangle>;

STriangles toTriangles(const SFace& face);
