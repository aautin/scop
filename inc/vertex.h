#pragma once

#include "types.h"

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
SDimension getDimension(const SVerticesVec &vertices);

//-------------------------------//
//- Complete vertex properties  -//
//-------------------------------//
void assignDistinguishableColors(const SObjectsMap& objects, SVerticesVec& vertices);
void assignTextureCoordinates(SVerticesVec &vertices, const SDimension &dimension);
void centerVerticesOnOrigin(SVerticesVec& vertices, const SDimension& dimension);
