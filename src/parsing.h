#pragma once

// STL headers
#include <string>

// Project headers
#include "geometric.h"

enum class ELineType
{
	Object,
	Vertex,
	Face,
	None,
};

class CParsing
{
public:
	static void removeComments(std::string& line);

	static ELineType   getLineType(const std::string& line);
	static std::string getObjectName(const std::string& line);
	static SVertex     getVertex(const std::string& line);
	static SFace       getFace(const std::string& line);
};
