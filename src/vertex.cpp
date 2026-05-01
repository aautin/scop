#include "vertex.h"

// STL headers
#include <random>
#include <filesystem>
#include <ranges>
#include <algorithm>

namespace rg = std::ranges;

static float randomFloat(float min, float max)
{
	static std::random_device rd;
    static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);

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
	SColorsSet usedColors;
	for (const auto& object : objects)
	{
		for (const auto& [materialName, triangles] : object.second.materialGroups)
		{
			for (const auto& triangle : triangles)
			{
				SColor color;
				do
				{
					color.r = randomFloat(0.0f, 1.0f);
					color.g = randomFloat(0.0f, 1.0f);
					color.b = randomFloat(0.0f, 1.0f);
				}
				while (usedColors.contains(color));

				usedColors.insert(color);
				for (size_t i = 0; i < 3; ++i)
				{
					vertices[triangle.vertexIndices[i]].color = color;
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
