#pragma once

// STL headers
#include <set>

// GLFW headers
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// Project headers
#include "scene.h"

class CUser
{
public:
	//-------------------------------//
	//- Constructors / Destructors  -//
	//-------------------------------//
	CUser() = default;
	~CUser() = default;

	//-------------------------------//
	//- Operations                  -//
	//-------------------------------//
	void loadFile(const std::string& filename);
	void handleKey(int key, int action);
	void updateScene();
	void render(GLFWwindow* window);
	
private:
	//-------------------------------//
	//- Data                        -//
	//-------------------------------//
	std::set<int> m_pPressedKeys;
	CScene	      m_pScene;
};
