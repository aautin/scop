#include "parsing.h"

// STL headers
#include <sstream>

//-------------------------------//
//- Operations                  -//
//-------------------------------//
void CParsing::removeComments(std::string& line)
{
	size_t commentPos = line.find('#');
	if (commentPos != std::string::npos)
	{
		line.erase(commentPos);
	}
}

CParsing::ELineType CParsing::getLineType(const std::string& line)
{
	if (line.starts_with("o "))
	{
		return ELineType::Object;
	}
	else if (line.starts_with("v "))
	{
		return ELineType::Vertex;
	}
	else if (line.starts_with("f "))
	{
		return ELineType::Face;
	}
	else
	{
		return ELineType::None;
	}
}

std::string CParsing::getObjectName(const std::string& line)
{
	return line.substr(2);
}

SVertex CParsing::getVertex(const std::string& line)
{
	SVertex vertex;
	sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	return vertex;
}

SFace CParsing::getFace(const std::string& line)
{
	SFace              face;
	std::istringstream iss(line.substr(2));
	std::string        vertexIndex;

	while (iss >> vertexIndex)
	{
		face.verticesIndices.push_back(std::stoul(vertexIndex));
	}
	return face;
}
