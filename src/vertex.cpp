#include "vertex.h"

// STL headers
#include <random>
#include <filesystem>
#include <ranges>
#include <algorithm>

namespace rg = std::ranges;

template <typename T>
static T random(T min, T max)
{
	static std::random_device rd;
    static std::mt19937 gen(rd());
	std::uniform_real_distribution<T> dis(min, max);

	return dis(gen);
}

SDimension getDimension(const SVerticesVec& vertices)
{
	SDimension dimension;
	dimension.minX = rg::min_element(vertices, {}, [](const SVertex& v){return v.position.x;})->position.x;
	dimension.maxX = rg::max_element(vertices, {}, [](const SVertex& v){return v.position.x;})->position.x;
	dimension.minY = rg::min_element(vertices, {}, [](const SVertex& v){return v.position.y;})->position.y;
	dimension.maxY = rg::max_element(vertices, {}, [](const SVertex& v){return v.position.y;})->position.y;
	dimension.minZ = rg::min_element(vertices, {}, [](const SVertex& v){return v.position.z;})->position.z;
	dimension.maxZ = rg::max_element(vertices, {}, [](const SVertex& v){return v.position.z;})->position.z;

	dimension.centerX = (dimension.minX + dimension.maxX) / 2.0f;
	dimension.centerY = (dimension.minY + dimension.maxY) / 2.0f;
	dimension.centerZ = (dimension.minZ + dimension.maxZ) / 2.0f;

	return dimension;
}

void assignDistinguishableColors(const SObjectsMap& objects, SVerticesVec& vertices)
{
	std::map<SPositionVertex, float> usedColors;
	for (const auto& object : objects)
	{
		for (const auto& [materialName, triangles] : object.second.materialGroups)
		{
			for (const auto& triangle : triangles)
			{
				SPositionVerticesVec trianglesVertices;
				for (size_t i = 0; i < 3; ++i)
				{
					trianglesVertices.push_back(vertices[triangle.vertexIndices[i]].position);
				}
				float color;
				do
				{
					color = random<float>(0.0f, 1.0f);
				}
				while (rg::any_of(trianglesVertices, [&](auto vertex) { return usedColors[vertex] == color; }));

				for (size_t i = 0; i < 3; ++i)
				{
					vertices[triangle.vertexIndices[i]].color = { color, color, color };
					usedColors[trianglesVertices[i]] = color;
				}
			}
		}
	}
}

void assignTextureCoordinates(SVerticesVec& vertices, const SDimension& dimension)
{
	for (SVertex& vertex : vertices)
	{
		vertex.texture.u = (vertex.position.x - dimension.minX) / dimension.width();
		vertex.texture.v = (vertex.position.y - dimension.minY) / dimension.height();
	}
}

void centerVerticesOnOrigin(SVerticesVec& vertices, const SDimension& dimension)
{
	for (SVertex& vertex : vertices)
	{
		vertex.position.x -= dimension.centerX;
		vertex.position.y -= dimension.centerY;
		vertex.position.z -= dimension.centerZ;
	}
}