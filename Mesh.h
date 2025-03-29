#ifndef MESH_H
#define MESH_H

#include "Assets/Includes/tiny_obj_loader.h"
#include "glm.hpp"
#include "Core.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	Mesh(std::string _path);
	~Mesh();

private:
	bool LoadMesh(std::string _path);
	void CreateVertexBuffer();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	std::vector<unsigned int> vertIndices;
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> uvIndices;


	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint elementBuffer;
	const int strideMult = 5;
};
#endif
