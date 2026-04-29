#include "objFile.h"

// STL headers
#include <sstream>
#include <fstream>
#include <optional>
#include <format>
#include <stdexcept>
#include <algorithm>
#include <ranges>
#include <random>

static float randomFloat(float min, float max)
{
	static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    return dis(gen);
}

static bool isSameColor(const SColor& color1, const SColor& color2)
{
	return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b;
}

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

static SPositionVertex getVertexPosition(const std::string& line)
{
	SPositionVertex vertex;
	sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	return vertex;
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
	
	SPositionVertices vertices;
	
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
			m_pObjects.push_back(getObjectName(line));
			break;
		}
		case CObjFile::ELineType::Vertex:
		{
			vertices.push_back(getVertexPosition(line));
			break;
		}
		case CObjFile::ELineType::Face:
		{
			if (m_pObjects.empty())
			{
				throw std::runtime_error("Face defined before object");
			}
			
			auto face = getFace(line);
			for (const auto& index : face.vertexIndices)
			{
				if (index == 0 || index > vertices.size())
				{
					throw std::runtime_error(std::format("Face with a non existing index : {} ({})", line, index));
				}
			}
			for (auto& triangle : toTriangles(face))
			{
				SColor color;
				do
				{
					color.r = randomFloat(0.0f, 1.0f);
					color.g = randomFloat(0.0f, 1.0f);
					color.b = randomFloat(0.0f, 1.0f);
				}
				while (std::ranges::find_if(m_pVertices, [color](const auto& vertex)
					{ return isSameColor(vertex.color, color); }) != m_pVertices.end());

				for (size_t i = 0; i < 3; ++i)
				{
					m_pVertices.push_back({ vertices[triangle.vertexIndices[i] - 1], color });
					triangle.vertexIndices[i] = m_pVertices.size() - 1;
				}
			}

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

CObjFile::SFace CObjFile::getFace(const std::string& line)
{
	SFace              face;
	std::istringstream iss(line.substr(2));
	std::string        vertexIndex;

	while (iss >> vertexIndex)
	{
		face.vertexIndices.push_back(std::stoul(vertexIndex));
	}
	return face;
}

STriangles CObjFile::toTriangles(const SFace& face)
{
	if (face.vertexIndices.size() < 3)
	{
		throw std::runtime_error("Trying to convert to triangles a face with less than 3 vertices");
	}

	if (face.vertexIndices.size() == 3)
	{
		return {{ face.vertexIndices[0], face.vertexIndices[1], face.vertexIndices[2]}};
	}

	STriangles result;
	for (size_t i = 0; i < face.vertexIndices.size() - 2; ++i)
	{
		result.push_back({
			face.vertexIndices[0],
			face.vertexIndices[i + 1],
			face.vertexIndices[i + 2],
		});
	}
	return result;
}
