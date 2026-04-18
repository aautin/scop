#include "geometric.h"

// STL headers
#include <stdexcept>

STriangles toTriangles(const SFace& face)
{
	if (face.verticesIndices.size() < 3)
	{
		throw std::runtime_error("Trying to convert to triangles a face with less than 3 vertices");
	}

	if (face.verticesIndices.size() == 3)
	{
		return {{ face.verticesIndices[0], face.verticesIndices[1], face.verticesIndices[2]}};
	}

	STriangles result;
	for (size_t i = 0; i < face.verticesIndices.size() - 2; i)
	{
		result.push_back({
			face.verticesIndices[0],
			face.verticesIndices[i + 1],
			face.verticesIndices[i + 2],
		});
	}
	return result;
}
