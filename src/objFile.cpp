#include "objFile.h"

// LIBC headers
#include <string.h>

// STL headers
#include <sstream>
#include <fstream>
#include <optional>
#include <format>
#include <stdexcept>
#include <algorithm>
#include <ranges>
#include <filesystem>
#include <iostream>

// GLFW headers
#include "glad.h"
#include "GLFW/glfw3.h"

// Project headers
#include "user.h"

namespace fs = std::filesystem;

//-------------------------------//
//- Parsing                     -//
//-------------------------------//
static std::string resolveMTLPath(const std::string &objFilePath, const std::string &mtllibPath)
{
	fs::path objDir = fs::path(objFilePath).parent_path();
	fs::path mtlPath = mtllibPath;

	if (!mtlPath.is_absolute())
	{
		mtlPath = objDir / mtlPath;
	}

	return fs::absolute(mtlPath).lexically_normal().string();
}

static void removeComments(std::string &line)
{
	size_t commentPos = line.find('#');
	if (commentPos != std::string::npos)
	{
		line.erase(commentPos);
	}
}

static SMaterialsMap parseMtlFile(const std::string &filename, const std::string &directory)
{
	SMaterialsMap materials;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error(std::format("Failed to open material file: {}", filename));
	}

	std::string currentMaterialName;
	for (std::string line; std::getline(file, line);)
	{
		removeComments(line);
		if (line.empty())
		{
			continue;
		}

		if (line.starts_with("newmtl "))
		{
			char buffer[256];
			sscanf(line.c_str(), "newmtl %s", buffer);
			currentMaterialName = buffer;
			materials[currentMaterialName] = SMaterial{};
		}
		else if (line.starts_with("Ka "))
		{
			sscanf(line.c_str(), "Ka %f %f %f", &materials[currentMaterialName].ambientColor.r,
				   &materials[currentMaterialName].ambientColor.g,
				   &materials[currentMaterialName].ambientColor.b);
		}
		else if (line.starts_with("Kd "))
		{
			sscanf(line.c_str(), "Kd %f %f %f", &materials[currentMaterialName].diffuseColor.r,
				   &materials[currentMaterialName].diffuseColor.g,
				   &materials[currentMaterialName].diffuseColor.b);
		}
		else if (line.starts_with("Ks "))
		{
			sscanf(line.c_str(), "Ks %f %f %f", &materials[currentMaterialName].specularColor.r,
				   &materials[currentMaterialName].specularColor.g,
				   &materials[currentMaterialName].specularColor.b);
		}
		else if (line.starts_with("Ns "))
		{
			sscanf(line.c_str(), "Ns %f", &materials[currentMaterialName].specularExponent);
		}
		else if (line.starts_with("Ni "))
		{
			sscanf(line.c_str(), "Ni %f", &materials[currentMaterialName].opticalDensity);
		}
		else if (line.starts_with("d "))
		{
			sscanf(line.c_str(), "d %f", &materials[currentMaterialName].dissolve);
		}
		else if (line.starts_with("illum "))
		{
			sscanf(line.c_str(), "illum %d", &materials[currentMaterialName].illuminationModel);
		}
	}
	return materials;
}

static std::string getObjectName(const std::string &line)
{
	return line.substr(2);
}

static SPositionVertex getPositionVertex(const std::string &line)
{
	SPositionVertex vertex;
	sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
	return vertex;
}

//-------------------------------//
//- Constructors / Destructors  -//
//-------------------------------//
CObjFile::CObjFile(const std::string &filename) : m_pUsedMaterials{{"default", SMaterial{}}},
												  m_pObjects{}
{
	if (filename.compare(filename.length() - 4, 4, ".obj"))
	{
		throw std::runtime_error("Only .obj files are supported");
	}
	if (!fs::exists(filename))
	{
		throw std::runtime_error(std::format("File {} does not exist", filename));
	}

	m_pFileName = filename;

	SPositionVerticesVec importedVertices;
	SMaterialsMap importedMaterials = m_pUsedMaterials;

	SMaterialName inUseMaterialName = m_pUsedMaterials.begin()->first;
	SObjectName inUseObjectName;
	std::optional<size_t> inUseSmoothingGroupIndex = std::nullopt;

	auto fileContent = std::ifstream(filename);
	char nameBuffer[256];
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
		{
			sscanf(line.c_str(), "mtllib %s", nameBuffer);
			auto materials = parseMtlFile(resolveMTLPath(filename, nameBuffer), fs::path(filename).parent_path().string());
			importedMaterials.insert(materials.begin(), materials.end());
			break;
		}
		case CObjFile::ELineType::Usemtl:
		{
			sscanf(line.c_str(), "usemtl %s", nameBuffer);
			if (!importedMaterials.contains(nameBuffer))
			{
				throw std::runtime_error(std::format("Material {} not found in ! file", nameBuffer));
			}

			if (!m_pUsedMaterials.contains(nameBuffer))
			{
				m_pUsedMaterials[nameBuffer] = importedMaterials[nameBuffer];
			}
			inUseMaterialName = nameBuffer;
			break;
		}
		case CObjFile::ELineType::SmoothingGroup:
		{
			sscanf(line.c_str(), "s %s", nameBuffer);
			if (strcmp(nameBuffer, "off") == 0)
			{
				inUseSmoothingGroupIndex = std::nullopt;
			}
			else
			{
				inUseSmoothingGroupIndex = std::stoul(nameBuffer);
			}
			break;
		}
		case CObjFile::ELineType::Object:
		{
			inUseObjectName = getObjectName(line);
			m_pObjects.try_emplace(inUseObjectName, SObject{.name = inUseObjectName});
			break;
		}
		case CObjFile::ELineType::Vertex:
		{
			importedVertices.push_back(getPositionVertex(line));
			break;
		}
		case CObjFile::ELineType::Face:
		{
			if (inUseObjectName.empty())
			{
				inUseObjectName = "default";
			}
			m_pObjects.try_emplace(inUseObjectName, SObject{.name = inUseObjectName});

			auto face = getFace(line);
			for (const auto &index : face.vertexIndices)
			{
				if (index == 0 || index > importedVertices.size())
				{
					throw std::runtime_error(std::format("Face with a non existing index : {} ({})", line, index));
				}
			}
			for (auto &triangle : toTriangles(face))
			{
				//
				// Push the triangle vertices to the used vertices
				// Update triangle vertex indices to match the used vertices list indices
				//
				for (size_t i = 0; i < 3; ++i)
				{
					m_pUsedVertices.push_back({inUseSmoothingGroupIndex,
											   importedVertices[triangle.vertexIndices[i] - 1]});
					triangle.vertexIndices[i] = m_pUsedVertices.size() - 1;
				}
				m_pObjects[inUseObjectName].materialGroups[inUseMaterialName].triangles.push_back(triangle);
			}

			break;
		}
		case CObjFile::ELineType::None:
			throw std::runtime_error(std::format("Unknown line type: {}", line));
		}
	}
}

void CObjFile::addToUser(SUser &user)
{
	//
	// Update triangle vertex indices to match the user vertices list indices
	//
	for (auto &object : m_pObjects)
	{
		for (auto &[materialName, materialContent] : object.second.materialGroups)
		{
			for (auto &triangle : materialContent.triangles)
			{
				for (size_t i = 0; i < 3; ++i)
				{
					triangle.vertexIndices[i] += user.vertices.size();
				}
			}
		}
	}

	//
	// Push the used vertices to the user vertices list
	//
	user.vertices.insert(user.vertices.end(), m_pUsedVertices.begin(), m_pUsedVertices.end());

	//
	// Push the file objects and materials to the user files
	//
	for (const auto &[objectName, object] : m_pObjects)
	{
		user.files[m_pFileName].objects[objectName] = object;
	}
	for (const auto &[materialName, material] : m_pUsedMaterials)
	{
		user.files[m_pFileName].materials[materialName] = material;
	}

	//
	// Define the EBO for each material group of each object
	//
	for (auto &[objectName, object] : user.files[m_pFileName].objects)
	{
		for (auto &[materialName, materialContent] : object.materialGroups)
		{
			if (!materialContent.triangles.empty())
			{
				glGenBuffers(1, &materialContent.ebo);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, materialContent.ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, materialContent.triangles.size() * sizeof(STriangle),
					materialContent.triangles.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
	}
}

//-------------------------------//
//- Internal operations         -//
//-------------------------------//
CObjFile::ELineType CObjFile::getLineType(const std::string &line)
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

CObjFile::SFace CObjFile::getFace(const std::string &line)
{
	SFace face;
	std::istringstream iss(line.substr(2));
	std::string vertexIndex;

	while (iss >> vertexIndex)
	{
		face.vertexIndices.push_back(std::stoul(vertexIndex));
	}
	return face;
}

STrianglesVec CObjFile::toTriangles(const SFace &face)
{
	if (face.vertexIndices.size() < 3)
	{
		throw std::runtime_error("Trying to convert to triangles a face with less than 3 vertices");
	}

	if (face.vertexIndices.size() == 3)
	{
		return {{face.vertexIndices[0], face.vertexIndices[1], face.vertexIndices[2]}};
	}

	STrianglesVec result;
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
