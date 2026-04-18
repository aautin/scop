#pragma once

// STL headers
#include <vector>
#include <string>
#include <map>

// Project headers
#include "geometric.h"

class CObject
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CObject(const std::string& name, const SVertexes& vertexes, const SFaces& faces);
	~CObject() = default;
	
private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	std::string               m_pName;
	STriangles                m_pTriangles;
	std::map<size_t, SVertex> m_pVertexes;
};
using CObjects = std::vector<CObject>;
