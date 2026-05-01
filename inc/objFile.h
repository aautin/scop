#pragma once

// STL headers
#include <string>

// Project headers
#include "types.h"

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
	SObjectsMap getObjects() const { return m_pObjects; }

	/*!
	 * @brief Used to fill the VBO (each vertex = position + color values)
	 */
	SVerticesVec getUsedVertices() const { return m_pUsedVertices; }

	/*!
	 * @brief Used to switch between materials when objects are drawn
	 */
	SMaterialsMap getUsedMaterials() const { return m_pUsedMaterials; }

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
	static ELineType     getLineType(const std::string& line);
	static SFace         getFace(const std::string& line);
	static STrianglesVec toTriangles(const SFace& face);

	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	SObjectsMap   m_pObjects;
	SVerticesVec  m_pUsedVertices;
	SMaterialsMap m_pUsedMaterials = {{"default", SMaterial{}}};
};
