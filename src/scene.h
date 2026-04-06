#pragma once

// STL headers
#include <vector>

// Project headers
#include "object.h"

class CScene
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CScene() = default;
	~CScene() = default;

	//-------------------------------//
	//- Parameters                  -//
	//-------------------------------//
	void addObject(const CObject& object) { m_pObjects.push_back(object); }

private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	CObjects m_pObjects;
};
