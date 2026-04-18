#include "user.h"

// STL headers
#include <iostream>
#include <fstream>
#include <optional>
#include <format>

// Project headers
#include "parsing.h"


//-------------------------------//
//- Operations                  -//
//-------------------------------//
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
	
	std::optional<std::string> objectName;
	SVertexes                  vertexes;
	SFaces                     faces;

	auto fileContent = std::ifstream(filename);
	for (std::string line; std::getline(fileContent, line);)
	{
		CParsing::removeComments(line);
		if (line.empty())
		{
			continue;
		}

		std::cout << line;
		CParsing::ELineType lineType = CParsing::getLineType(line);
		switch (lineType)
		{
		case CParsing::Object:
		{
			if (objectName)
			{
				try
				{
					CObject object(*objectName, vertexes, faces);
					m_pScene.addObject(object);
				}
				catch (const std::exception& e)
				{
					std::cerr << "Error loading object: " << e.what() << std::endl;
				}

				objectName.reset();
			}
			
			faces.clear();
			objectName = CParsing::getObjectName(line);
			break;
		}
		case CParsing::Vertex:
		{
			if (!objectName)
			{
				throw std::runtime_error("Vertex defined before object");
			}

			vertexes.push_back(CParsing::getVertex(line));
			break;
		}
		case CParsing::Face:
		{
			if (!objectName)
			{
				throw std::runtime_error("Face defined before object");
			}
			
			auto face = CParsing::getFace(line);
			for (const auto& index : face.verticesIndices)
			{
				if (index > vertexes.size())
				{
					throw std::runtime_error(std::format("Face with a non existing index : {} ({})", line, index));
				}
			}

			faces.push_back(face);
			break;
		}
		case CParsing::None:
			throw std::runtime_error("Unknown line type");
		}
	}

	if (objectName)
	{
		try
		{
			CObject object(*objectName, vertexes, faces);
			m_pScene.addObject(object);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error loading object: " << e.what() << std::endl;
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