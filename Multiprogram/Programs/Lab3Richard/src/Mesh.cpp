#include "Mesh.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "Ps2.hpp"

Mesh::Mesh(std::string&& fileName)
{
	load(fileName);
}

Mesh::Mesh(std::string& fileName)
{
	load(fileName);
}

Mesh::~Mesh()
{
}

void Mesh::load(std::string&& fileName)
{
	load(fileName);
}

void Mesh::load(std::string& fileName)
{
	std::cout << "Loading mesh \"" << fileName << "\"" << std::endl;

	std::vector<glm::vec4>		vertices;
	std::vector<unsigned int>	indices;

	loadMeshData(fileName, vertices, indices);

	convert(vertices, indices);
	
	//std::cout << "Lines:" << std::endl;
	//for(unsigned int i = 0; i < lines.size(); i += 2) {
	//	glm::vec4 a = lines[i];
	//	glm::vec4 b = lines[i + 1];
	//	
	//	std::cout << "        (" << a.x << "," << a.y << "," << a.z << ","
	//		<< a.w << ")    to    (" << b.x << "," << b.y << "," << b.z << ","
	//		<< b.w << std::endl;
	//}

	std::cout << "    Number of tris: " << numTris << std::endl;
	std::cout << "    Number of verts: " << verts.size() << std::endl;
	std::cout << "    Memory Footprint: " << numBytes << std::endl;

	std::cout << "    Done!" << std::endl;
}

void Mesh::convert(
	std::vector<glm::vec4>& vertices,
	std::vector<unsigned int>& indices)
{
	for(unsigned int i = 0; i < indices.size(); i += 3) {
		unsigned int i1 = indices[i];
		unsigned int i2 = indices[i + 1];
		unsigned int i3 = indices[i + 2];

		verts.push_back(vertices[i1]);
		verts.push_back(vertices[i2]);
		verts.push_back(vertices[i3]);
	}

	numTris = verts.size() / 3;
	numBytes = verts.size() * sizeof(glm::vec4);
}

void Mesh::loadMeshData(
	std::string& fileName,
	std::vector<glm::vec4>& vertices,
	std::vector<unsigned int>& indices) 
{
	try {

		std::cout << "    Opening file" << std::endl;

		std::ifstream file(fileName);
		if (!file.is_open()) {
			std::cerr << "    Could not open mesh \"" << fileName << "\"" << std::endl;
			return;
		}

		// For each line in the obj file
		std::cout << "    Reading in file data" << std::endl;
		for (std::string line; std::getline(file, line); )
		{
			std::stringstream lineStream(line);

			std::string type;
			lineStream >> type;

			if (type == "v") {
				glm::vec4 pos{ 0, 0, 0, 1 };
				lineStream >> pos.x >> pos.y >> pos.z;

				vertices.push_back(pos);
			}
			else if (type == "f") {
				// Each triangle has 3 indices, so we fetch 3. This would be faster
				// unrolled, but this creates shorter code.
				for (int i = 0; i < 3; i++) {
					unsigned int index;

					lineStream >> index;

					indices.push_back(index - 1);
				}
			}
		}
	}
	catch (...) {
		std::cerr << "    Unknown issue loading file data!" << std::endl;
	}
}

void Mesh::draw(glm::mat4& matTrans, glm::vec3 color)
{
	glm::vec4 c{color.r, color.g, color.b, 1.0f};
	ps2DrawTrianglesWireframe(verts, matTrans, c);
}