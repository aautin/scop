#pragma once

// STL headers
#include <vector>
#include <string>
#include <map>
#include <set>

// Project headers
#include "types.h"

class CObject
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CObject(const std::string& name): m_pName(name) {}
	~CObject() = default;
	
	//-------------------------------//
	//- Parameters                  -//
	//-------------------------------//
	/*!
	 * @brief Used to load materials in OpenGL, then fill the EBO with their indices
	 */
	SMaterialGroups getMaterialGroups() const { return m_pMaterialGroups; }

	/*!
	 * @brief Get the name of the object, useful for debugging
	 */
	std::string getName() const { return m_pName; }
	
	/*!
	 * @brief Add a material group with its triangles and material properties
	 */
	void addMaterialGroup(const SMaterialGroup& group) { m_pMaterialGroups.push_back(group); }

private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	std::string     m_pName;
	SMaterialGroups m_pMaterialGroups;
};
using CObjects = std::vector<CObject>;
