#include "objFile.h"

// STL headers
#include <sstream>
#include <fstream>
#include <optional>
#include <format>

static void removeComments(std::string& line)
{
	size_t commentPos = line.find('#');
	if (commentPos != std::string::npos)
	{
		line.erase(commentPos);
	}
}

static std::string getObjectName(const std::string& line)
{
	return line.substr(2);
}

static SVertex getVertex(const std::string& line)
{
	SVertex vertex;
	sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	return vertex;
}

static SFace getFace(const std::string& line)
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

//-------------------------------//
//- Constructors / Destructors  -//
//-------------------------------//
CObjFile::CObjFile(const std::string& filename)
{
	if (filename.compare(filename.length() - 4, 4, ".obj"))
	{
		throw std::runtime_error("Only .obj files are supported");
	}
	
	std::optional<std::string> objectName;
	SVertices                  vertices;
	SFaces                     faces;

	auto fileContent = std::ifstream(filename);
	for (std::string line; std::getline(fileContent, line);)
	{
		removeComments(line);
		if (line.empty())
		{
			continue;
		}

		CObjFile::ELineType lineType = getLineType(line);
		switch (lineType)
		{
		case CObjFile::ELineType::Mtllib:
		case CObjFile::ELineType::Usemtl:
		case CObjFile::ELineType::SmoothingGroup:
		{
			//
			// Ignored for now
			//
			break;
		}
		case CObjFile::ELineType::Object:
		{
			if (objectName)
			{
				m_pObjects.push_back(CObject(*objectName, faces));
				objectName.reset();
			}
			
			faces.clear();
			objectName = getObjectName(line);
			break;
		}
		case CObjFile::ELineType::Vertex:
		{
			if (!objectName)
			{
				throw std::runtime_error("Vertex defined before object");
			}

			vertices.push_back(getVertex(line));
			break;
		}
		case CObjFile::ELineType::Face:
		{
			if (!objectName)
			{
				throw std::runtime_error("Face defined before object");
			}
			
			auto face = getFace(line);
			for (const auto& index : face.verticesIndices)
			{
				if (index == 0 || index > vertices.size())
				{
					throw std::runtime_error(std::format("Face with a non existing index : {} ({})", line, index));
				}
			}

			faces.push_back(face);
			break;
		}
		case CObjFile::ELineType::None:
			throw std::runtime_error(std::format("Unknown line type: {}", line));
		}
	}

	if (objectName)
	{
		m_pObjects.push_back(CObject(*objectName, faces));
	}

	m_pVertices = vertices;
}

//-------------------------------//
//- Internal operations         -//
//-------------------------------//
CObjFile::ELineType CObjFile::getLineType(const std::string& line)
{
	if (line.starts_with("o "))
	{
		return CObjFile::ELineType::Object;
	}
	else if (line.starts_with("v "))
	{
		return CObjFile::ELineType::Vertex;
	}
	else if (line.starts_with("f "))
	{
		return CObjFile::ELineType::Face;
	}
	else if (line.starts_with("mtllib "))
	{
		return CObjFile::ELineType::Mtllib;
	}
	else if (line.starts_with("usemtl "))
	{
		return CObjFile::ELineType::Usemtl;
	}
	else if (line.starts_with("s "))
	{
		return CObjFile::ELineType::SmoothingGroup;
	}
	else
	{
		return CObjFile::ELineType::None;
	}
}