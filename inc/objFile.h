#pragma once

// STL headers
#include <string>

// Project headers
#include "types.h"
#include "object.h"

class CObjFile
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CObjFile(const std::string& filename);
	~CObjFile() = default;

	//-------------------------------//
	//- Parameters                  -//
	//-------------------------------//
	/*!
	 * @brief Used to iterate over objects and draw each of them
	 */
	CObjects getObjects() const { return m_pObjects; }

	/*!
	 * @brief Used to fill the VBO (each vertex = position + color values)
	 */
	SVertices getVertices() const { return m_pVertices; }

private:
	//-------------------------------//
	//- Internal definitions        -//
	//-------------------------------//
	enum ELineType
	{
		Object,
		Vertex,
		Face,
		
		Mtllib,
		Usemtl,
		SmoothingGroup,

		None,
	};

	struct SFace
	{
		std::vector<size_t> vertexIndices;
	};
	using SFaces = std::vector<SFace>;

	//-------------------------------//
	//- Internal operations         -//
	//-------------------------------//
	static ELineType  getLineType(const std::string& line);
	static SFace      getFace(const std::string& line);
	static STriangles toTriangles(const SFace& face);

	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	CObjects  m_pObjects;
	SVertices m_pVertices;
};
