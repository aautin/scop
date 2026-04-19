#pragma once

// STL headers
#include <string>

// Project headers
#include "geometric.h"
#include "object.h"

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

class CFile
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CFile(const std::string& filename);
	~CFile() = default;

	//-------------------------------//
	//- Parameters                  -//
	//-------------------------------//
	CObjects getObjects() const { return m_pObjects; }
	SVertices getVertices() const { return m_pVertices; }

private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	CObjects  m_pObjects;
	SVertices m_pVertices;
};
