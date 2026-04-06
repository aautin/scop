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
using SVertexes = std::vector<SVertex>;

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

float epsilon();

bool areCoplanar(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d);
bool areConvex(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d);
STriangles getTriangles(const SVertex& a, const SVertex& b, const SVertex& c, const SVertex& d);