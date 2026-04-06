#include "object.h"

// STL headers
#include <stdexcept>

//-------------------------------//
//- Constructors / Destructors  -//
//-------------------------------//
CObject::CObject(const std::string& name, const SVertexes& vertexes, const SFaces& faces)
: m_pName(name), m_pVertexes(vertexes)
{
	for (const SFace& face : faces)
	{
		if (face.verticesIndices.size() == 3)
		{
			m_pTriangles.push_back({
				face.verticesIndices[0],
				face.verticesIndices[1],
				face.verticesIndices[2]
			});
		}
		else if (face.verticesIndices.size() == 4)
		{
			STriangle triangle1 = {
				face.verticesIndices[0],
				face.verticesIndices[1],
				face.verticesIndices[2]
			};
			
			STriangle triangle2 = {
				face.verticesIndices[0],
				face.verticesIndices[2],
				face.verticesIndices[3]
			};
		}
		else
		{
			throw std::runtime_error("Face must have 3 or 4 vertices");
		}
	}
}
