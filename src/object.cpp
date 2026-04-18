#include "object.h"

// STL headers
#include <ranges>
#include <iostream>

//-------------------------------//
//- Constructors / Destructors  -//
//-------------------------------//
CObject::CObject(const std::string& name, const SVertexes& vertexes, const SFaces& faces)
: m_pName(name)
{
	for (const SFace& face : faces)
	{
		auto triangles = toTriangles(face);
		m_pTriangles.insert(m_pTriangles.begin(), triangles.begin(), triangles.end());
	}
	
	for (size_t i = 0; i < vertexes.size(); ++i)
	{
		if (std::ranges::find_if(m_pTriangles, [i](const STriangle& it) { return
			it.verticesIndices[0] == i ||
			it.verticesIndices[1] == i ||
			it.verticesIndices[2] == i; }) != m_pTriangles.end())
		{
			m_pVertexes[i] = vertexes[i];
		}
	}

	std::cout << "Object name: " << name << std::endl;
	for (const auto& triangle : m_pTriangles)
	{
		std::cout << "Triangle " << triangle.verticesIndices[0] << triangle.verticesIndices[1] << triangle.verticesIndices[2] << std::endl;
	}
	std::cout << "-------------" << std::endl;
}
