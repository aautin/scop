#pragma once

// STL headers
#include <string>
#include <optional>
#include <vector>
#include <limits>
#include <cmath>
#include <map>
#include <set>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

// Project headers
#include "math3d.h"

struct SPositionVertex
{
	float x, y, z;
	bool operator==(const auto& v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator<(const auto& v) const
	{
		if (x != v.x) return x < v.x;
		if (y != v.y) return y < v.y;
		return z < v.z;
	}
};
using SPositionVerticesVec = std::vector<SPositionVertex>;

struct STextureVertex
{
	float u, v;
};

struct SNormalVertex
{
	float x, y, z;
};

struct SDimension
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	float centerX, centerY, centerZ;

	float width() const { return maxX - minX; }
	float height() const { return maxY - minY; }
	float depth() const { return maxZ - minZ; }
};

struct SColor
{
	float r, g, b;
	bool operator==(const auto& c) const { return r == c.r && g == c.g && b == c.b; }
	bool operator<(const auto& c) const
	{
		if (r != c.r) return r < c.r;
		if (g != c.g) return g < c.g;
		return b < c.b;
	}
};
using SColorsSet = std::set<SColor>;

struct SVertex
{
	std::optional<size_t> smoothingGroupIndex;
	SPositionVertex       position;
	SColor                color;
	STextureVertex        texture;
	SNormalVertex         normal;
	bool                  isSmoothed = false;
};
using SVerticesVec = std::vector<SVertex>;

struct STriangle
{
	uint32_t vertexIndices[3];
};
using STrianglesVec = std::vector<STriangle>;

//--------------------------------//
//- Material                     -//
//--------------------------------//
using SMaterialName = std::string;
struct SMaterial
{
	SColor ambientColor  = {0.2, 0.2, 0.2}; // Ka
	SColor diffuseColor  = {0.8, 0.8, 0.8}; // Kd
	SColor specularColor = {1, 1, 1};       // Ks
	float  specularExponent  = 20;          // Ns
	float  opticalDensity    = 1;           // Ni
	float  dissolve          = 1;           // d
	int    illuminationModel = 2;           // illum
};
struct SPerMaterialContent
{
	STrianglesVec triangles;
	GLuint        ebo;
};

using SMaterialsMap = std::map<SMaterialName, SMaterial>;
using SMaterialGroupsMap = std::map<SMaterialName, SPerMaterialContent>;

//--------------------------------//
//- Object                       -//
//--------------------------------//
using SObjectName = std::string;
struct SObject
{
	SObjectName        name;
	SMaterialGroupsMap materialGroups;
	SMat4              translation = identityMat4();
	SMat4              rotation = identityMat4();
	SDimension         dimension;
};
using SObjectsMap = std::map<SObjectName, SObject>;

//-------------------------------//
//- File                        -//
//-------------------------------//
struct SFile
{
	SObjectsMap   objects;
	SMaterialsMap materials;
};
using SFilesMap = std::map<std::string, SFile>;