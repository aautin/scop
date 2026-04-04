#include "user.h"

// STL headers
#include <fstream>
#include <optional>

// Project headers
#include "parsing.h"

void CUser::handleKey(int key, int action)
{
	if (action == GLFW_PRESS)
	{
		m_pPressedKeys.insert(key);
	}
	else if (action == GLFW_RELEASE)
	{
		m_pPressedKeys.erase(key);
	}
}

void CUser::loadFile(const std::string& filename)
{
	if (filename.compare(filename.length() - 5, 5, ".obj"))
	{
		throw std::runtime_error("Only .obj files are supported");
	}
	
	auto fileContent = std::ifstream(filename);
	for (std::string line; std::getline(fileContent, line);)
	{
		std::optional<std::string> objectName;
		SVertexes   vertexes;
		SFaces      faces;
		
		CParsing::removeComments(line);
		if (line.empty())
		{
			continue;
		}

		ELineType lineType = CParsing::getLineType(line);
		switch (lineType)
		{
		case ELineType::Object
		{
			object = CParsing::getObjectName(line);
			break;
		}
		case ELineType::Vertex:
		{
			if (!objectName)
			{
				throw std::runtime_error("Vertex defined before object");
			}

			vertexes.push_back(CParsing::getVertex(line));
			break;
		}
		case ELineType::Face:
		{
			if (!objectName)
			{
				throw std::runtime_error("Face defined before object");
			}

			auto face = CParsing::getFace(line);
			for (unsigned int i = 0; i < 3; ++i)
			{
				if(face.vertexIndices[i] > vertexes.size())
				{
					throw std::runtime_error("Face references non-existent vertex");
				}
			}
			faces.push_back(face);
			break;
		}
		case ELineType::None:
			throw std::runtime_error("Unknown line type");
		}
	}
}

void CUser::updateScene()
{
	//
	// To be continued
	//
}

void CUser::render(GLFWwindow* window)
{
	//
	// To be continued
	//
}