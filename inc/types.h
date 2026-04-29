#pragma once

// STL headers
#include <string>
#include <optional>
#include <vector>
#include <limits>
#include <cmath>

struct SPositionVertex
{
	float x, y, z;
};
using SPositionVertices = std::vector<SPositionVertex>;

struct SColor
{
	float r, g, b;
};
using SColors = std::vector<SColor>;

struct SVertex
{
	SPositionVertex position;
	SColor          color;
};
using SVertices = std::vector<SVertex>;

struct STriangle
{
	size_t vertexIndices[3];
};
using STriangles = std::vector<STriangle>;

struct SMaterial
{
	std::string           name;              // newmtl
	std::optional<SColor> ambientColor;      // Ka
	std::optional<SColor> diffuseColor;      // Kd
	std::optional<SColor> specularColor;     // Ks
	std::optional<float>  specularExponent;  // Ns
	std::optional<float>  opticalDensity;    // Ni
	std::optional<float>  dissolve;          // d
	int    illuminationModel = 1; // illum
};

struct SMaterialGroup
{
	SMaterial  material;
	STriangles triangles;
};
using SMaterialGroups = std::vector<SMaterialGroup>;
