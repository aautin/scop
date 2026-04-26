#pragma once

// STL headers
#include <string>

// Project headers
#include "geometric.h"
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
	CObjects getObjects() const { return m_pObjects; }
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

	//-------------------------------//
	//- Internal operations         -//
	//-------------------------------//
	static ELineType getLineType(const std::string& line);

	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	CObjects  m_pObjects;
	SVertices m_pVertices;
};
