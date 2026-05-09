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
void assignNormals(const SObjectsMap& objects, SVerticesVec& vertices,  const bool smooth);
void centerVerticesOnOrigin(SVerticesVec& vertices, const SDimension& dimension);

/*!
 * @brief  Assign texture coords to vertices based on position, normal and model dimensions 
 * @throws std::runtime_error if a vertex has no assigned normal
*/
void assignTextureCoordinates(SVerticesVec &vertices, const SDimension &dimension);