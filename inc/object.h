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
	CObject(const std::string& name, const SFaces& faces);
	~CObject() = default;
	
	//-------------------------------//
	//- Parameters                  -//
	//-------------------------------//
	STriangles getTriangles() const { return m_pTriangles; }

private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	std::string               m_pName;
	STriangles                m_pTriangles;
};
using CObjects = std::vector<CObject>;
