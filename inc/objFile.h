#pragma once

// STL headers
#include <string>

// Project headers
#include "types.h"

// Forward declarations
struct SUser;

class CObjFile
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CObjFile(const std::string& filename);
	~CObjFile() = default;

	//-------------------------------//
	//- Operations                  -//
	//-------------------------------//
	void addToUser(SUser& user);

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
		std::vector<uint32_t> vertexIndices;
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
	std::string   m_pFileName;
	SObjectsMap   m_pObjects;
	SVerticesVec  m_pUsedVertices;
	SMaterialsMap m_pUsedMaterials = {{"default", SMaterial{}}};
};
