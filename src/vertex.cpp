#include "vertex.h"

// STL headers
#include <random>
#include <filesystem>
#include <ranges>
#include <algorithm>
#include <iostream>

// Project headers
#include "math3d.h"
#include "types.h"
#include "user.h"

namespace rg = std::ranges;

template <typename T>
static T random(T min, T max)
{
	static std::random_device rd;
    static std::mt19937 gen(rd());
	std::uniform_real_distribution<T> dis(min, max);

	return dis(gen);
}

static SDimension setDimension(SObject& object, const SVerticesVec& vertices)
{
	object.dimension.minX = object.dimension.minY = object.dimension.minZ = std::numeric_limits<float>::max();
	object.dimension.maxX = object.dimension.maxY = object.dimension.maxZ = std::numeric_limits<float>::lowest();
	for (const auto& materialGroup : object.materialGroups)
	{
		for (const auto& triangle : materialGroup.second.triangles)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				const auto& vertex = vertices[triangle.vertexIndices[i]];
				object.dimension.minX = std::min(object.dimension.minX, vertex.position.x);
				object.dimension.maxX = std::max(object.dimension.maxX, vertex.position.x);
				object.dimension.minY = std::min(object.dimension.minY, vertex.position.y);
				object.dimension.maxY = std::max(object.dimension.maxY, vertex.position.y);
				object.dimension.minZ = std::min(object.dimension.minZ, vertex.position.z);
				object.dimension.maxZ = std::max(object.dimension.maxZ, vertex.position.z);
			}
		}
	}

	object.dimension.centerX = (object.dimension.minX + object.dimension.maxX) / 2.0f;
	object.dimension.centerY = (object.dimension.minY + object.dimension.maxY) / 2.0f;
	object.dimension.centerZ = (object.dimension.minZ + object.dimension.maxZ) / 2.0f;

	return object.dimension;
}

void assignDistinguishableColors(const SObject& object, SVerticesVec& vertices)
{
	std::map<SPositionVertex, float> usedColors;
	for (const auto& materialGroup : object.materialGroups)
	{
		for (const auto& triangle : materialGroup.second.triangles)
		{
			SPositionVerticesVec trianglesVertices;
			for (size_t i = 0; i < 3; ++i)
			{
				trianglesVertices.push_back(vertices[triangle.vertexIndices[i]].position);
				if (usedColors.contains(trianglesVertices.back()))
				{
					continue;
				}
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

void assignNormals(const SObject& object, SVerticesVec& vertices, const bool smooth)
{
	//
	// For each triangle, we calculate clockwise and assign the normal vector to its 3 vertices
	// No smoothing is applied, to be continued...
	//

	for (const auto& [materialName, materialContent] : object.materialGroups)
	{
		for (const auto& triangle : materialContent.triangles)
		{
			SVec3 v0 = { vertices[triangle.vertexIndices[0]].position.x,
							vertices[triangle.vertexIndices[0]].position.y,
							vertices[triangle.vertexIndices[0]].position.z };
			SVec3 v1 = { vertices[triangle.vertexIndices[1]].position.x,
							vertices[triangle.vertexIndices[1]].position.y,
							vertices[triangle.vertexIndices[1]].position.z };
			SVec3 v2 = { vertices[triangle.vertexIndices[2]].position.x,
							vertices[triangle.vertexIndices[2]].position.y,
							vertices[triangle.vertexIndices[2]].position.z };

			SVec3 normal = normalize(cross(v1 - v0, v2 - v0));
			for (size_t i = 0; i < 3; ++i)
			{
				vertices[triangle.vertexIndices[i]].normal = { normal.x, normal.y, normal.z };
			}
		}
	}

	if (smooth)
	{
		//
		// For each vertex position, the normal is the average of all vertices normals
		// with the same position and the same smoothing group index
		//
		for (SVertex& vertex : vertices)
		{
			if (!vertex.smoothingGroupIndex.has_value() || vertex.isSmoothed)
			{
				continue;
			}

			SVec3 normalSum;
			size_t count = 0;

			for (SVertex& v : vertices)
			{
				if (v.position == vertex.position
					&& v.smoothingGroupIndex.has_value()
					&& v.smoothingGroupIndex == vertex.smoothingGroupIndex)
				{
					normalSum += SVec3{v.normal.x, v.normal.y, v.normal.z};
					++count;
				}
			}

			if (count > 0)
			{
				normalSum /= static_cast<float>(count);

				for (SVertex& v : vertices)
				{
					if (v.position == vertex.position && v.smoothingGroupIndex == vertex.smoothingGroupIndex)
					{
						v.normal = { normalSum.x, normalSum.y, normalSum.z };
						v.isSmoothed = true;
					}
				}
			}
		}
	}
}

void centerVerticesOnOrigin(const SObject& object, SVerticesVec& vertices)
{
	for (auto& materialGroup : object.materialGroups)
	{
		for (auto& triangle : materialGroup.second.triangles)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				vertices[triangle.vertexIndices[i]].position.x -= object.dimension.centerX;
				vertices[triangle.vertexIndices[i]].position.y -= object.dimension.centerY;
				vertices[triangle.vertexIndices[i]].position.z -= object.dimension.centerZ;
			}
		}
	}
}

void assignTextureCoordinates(const SObject& object, SVerticesVec& vertices)
{
	for (const auto& [materialName, materialContent] : object.materialGroups)
	{
		for (const auto& triangle : materialContent.triangles)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				SVertex& vertex = vertices[triangle.vertexIndices[i]];
				if (vertex.normal.x == 0 && vertex.normal.y == 0 && vertex.normal.z == 0)
				{
					throw std::runtime_error("Trying to assign texture coordinates to a vertex with no normal");
				}

				if (std::abs(vertex.normal.x) > std::abs(vertex.normal.y) && std::abs(vertex.normal.x) > std::abs(vertex.normal.z))
				{
					// Projection on yz plane
					vertex.texture.u = (vertex.position.z - object.dimension.minZ) / object.dimension.depth();
					vertex.texture.v = (vertex.position.y - object.dimension.minY) / object.dimension.height();
				}
				else if (std::abs(vertex.normal.y) > std::abs(vertex.normal.x) && std::abs(vertex.normal.y) > std::abs(vertex.normal.z))
				{
					// Projection on xz plane
					vertex.texture.u = (vertex.position.x - object.dimension.minX) / object.dimension.width();
					vertex.texture.v = (vertex.position.z - object.dimension.minZ) / object.dimension.depth();
				}
				else
				{
					// Projection on xy plane
					vertex.texture.u = (vertex.position.x - object.dimension.minX) / object.dimension.width();
					vertex.texture.v = (vertex.position.y - object.dimension.minY) / object.dimension.height();
				}
			}
		}
	}
}

void fillVertices(SUser& user)
{
	for (auto& [fileName, file] : user.files)
	{
		for (auto& [objectName, object] : file.objects)
		{
			setDimension(object, user.vertices);
			assignDistinguishableColors(object, user.vertices);
			assignNormals(object, user.vertices, true);
			centerVerticesOnOrigin(object, user.vertices);
			assignTextureCoordinates(object, user.vertices);
		}
	}
}

void placeObjectsSideBySide(SUser& user)
{
	float offset = 0.0f;
	for (auto& [fileName, file] : user.files)
	{
		for (auto& [objectName, object] : file.objects)
		{
			object.translation = translate(object.translation, { offset + object.dimension.width() / 2, 0.0f, 0.0f });
			offset += object.dimension.width() + 1.0f;
		}
	}
}
