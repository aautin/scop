#pragma once

// STL headers
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

// Project headers
#include "glad.h"

std::string get_file_contents(const char* filename);

class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile);

	//-------------------------------//
	//- Operations                  -//
	//-------------------------------//
	void activate();
	void destroy();

private:
	//--------------------------------//
	//- Data                         -//
	//--------------------------------//
	GLuint m_pId;
};