#pragma once

// STL headers
#include <string>
#include <optional>
#include <vector>
#include <limits>
#include <cmath>
#include <map>
#include <set>

struct SPositionVertex
{
	float x, y, z;
	bool operator==(const auto& v) const { return x == v.x && y == v.y && z == v.z; }
};
using SPositionVerticesVec = std::vector<SPositionVertex>;

struct STextureVertex
{
	float u, v;
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
	SPositionVertex position;
	SColor          color;
	STextureVertex  texture;
};
using SVerticesVec = std::vector<SVertex>;

struct STriangle
{
	size_t vertexIndices[3];
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
	SColor specularColor = {0, 0, 0};       // Ks
	float  specularExponent  = 1;           // Ns
	float  opticalDensity    = 1;           // Ni
	float  dissolve          = 1;           // d
	int    illuminationModel = 2;           // illum
};
using SMaterialsMap = std::map<SMaterialName, SMaterial>;
using SMaterialGroupsMap = std::map<SMaterialName, STrianglesVec>;

//--------------------------------//
//- Object                       -//
//--------------------------------//
using SObjectName = std::string;
struct SObject
{
	SObjectName        name;
	SMaterialGroupsMap materialGroups;
};
using SObjectsMap = std::map<SObjectName, SObject>;
