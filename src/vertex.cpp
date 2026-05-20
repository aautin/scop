#include "vertex.h"

// STL headers
#include <random>
#include <filesystem>
#include <ranges>
#include <algorithm>
#include <cmath>
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

void assignNormals(const SObject& object, SVerticesVec& vertices)
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

			SVec3 nCross = cross(v1 - v0, v2 - v0);
			float nLen = std::sqrt(nCross.x * nCross.x + nCross.y * nCross.y + nCross.z * nCross.z);
			if (nLen < 1e-8f)
			{
				// Degenerate/collinear triangle, skip normal assignment for this face
				continue;
			}

			SVec3 normal = normalize(nCross);
			for (size_t i = 0; i < 3; ++i)
			{
				vertices[triangle.vertexIndices[i]].normal = { normal.x, normal.y, normal.z };
			}
		}
	}
}

void smoothNormals(const SObject& object, SVerticesVec& vertices)
{
	// Group vertices by quantized position + smoothing group to avoid O(V^2)
	const float QUANT = 1e6f;
	std::map<std::tuple<long long, long long, long long, int>, std::vector<size_t>> groups;

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		SVertex& vertex = vertices[i];
		if (!vertex.smoothingGroupIndex.has_value())
			continue;

		long long qx = std::llround(vertex.position.x * QUANT);
		long long qy = std::llround(vertex.position.y * QUANT);
		long long qz = std::llround(vertex.position.z * QUANT);
		int sg = vertex.smoothingGroupIndex.value();

		groups[std::make_tuple(qx, qy, qz, sg)].push_back(i);
	}

	for (auto& [key, idxs] : groups)
	{
		SVec3 normalSum = { 0.0f, 0.0f, 0.0f };
		for (size_t idx : idxs)
		{
			normalSum += SVec3{ vertices[idx].normal.x, vertices[idx].normal.y, vertices[idx].normal.z };
		}

		if (idxs.empty())
			continue;

		normalSum /= static_cast<float>(idxs.size());

		float len = std::sqrt(normalSum.x * normalSum.x + normalSum.y * normalSum.y + normalSum.z * normalSum.z);
		if (len < 1e-8f)
			continue;

		normalSum = normalize(normalSum);

		for (size_t idx : idxs)
		{
			vertices[idx].normal = { normalSum.x, normalSum.y, normalSum.z };
			vertices[idx].isSmoothed = true;
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
					//
					// Projection on yz plane
					//
					vertex.texture.u = (vertex.position.z - object.dimension.minZ) / object.dimension.depth();
					vertex.texture.v = (vertex.position.y - object.dimension.minY) / object.dimension.height();
				}
				else if (std::abs(vertex.normal.y) > std::abs(vertex.normal.x) && std::abs(vertex.normal.y) > std::abs(vertex.normal.z))
				{
					//
					// Projection on xz plane
					//
					vertex.texture.u = (vertex.position.x - object.dimension.minX) / object.dimension.width();
					vertex.texture.v = (vertex.position.z - object.dimension.minZ) / object.dimension.depth();
				}
				else
				{
					//
					// Projection on xy plane
					//
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
			assignNormals(object, user.vertices);
			centerVerticesOnOrigin(object, user.vertices);
			assignTextureCoordinates(object, user.vertices);
			smoothNormals(object, user.vertices);
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
