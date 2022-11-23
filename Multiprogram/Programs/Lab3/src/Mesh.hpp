#ifndef MESH_HPP_INCLUDED
#define MESH_HPP_INCLUDED

#include <string>
#include <vector>


#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

/*
 * This class abstracts a mesh used in the game. It loads the data from a
 * .obj file and stores it onto the gpu.
 * 
 * More info on .obj files:
 *		https://en.wikipedia.org/wiki/Wavefront_.obj_file
 *		https://www.cs.cmu.edu/~mbz/personal/graphics/obj.html
 * 
 * This abstraction assumes that faces only have a position attribute, and
 * don't have a normal and texture index exported for each of the faces.
 */
class Mesh
{
public:
	Mesh() { }
	~Mesh();

	// These constructors/methods load the .obj file with the passed filename
	// and store them on the GPU
	Mesh(std::string&& fileName);
	Mesh(std::string& fileName);
	void load(std::string&& fileName);
	void load(std::string& fileName);

	void draw(glm::mat4& matTrans, glm::vec3 color);

private:
	// This method loads the data from the file and places it in the two
	// std::vector that are passed.
	void loadMeshData(
		std::string& fileName,
		std::vector<glm::vec4>& vertices,
		std::vector<unsigned int>& indices);
	
	void convert(
		std::vector<glm::vec4>& vertices,
		std::vector<unsigned int>& indices);

	std::vector<glm::vec4> verts;

	unsigned int numTris;
	unsigned int numBytes;
};


#endif//MESH_HPP_INCLUDED