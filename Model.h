#ifndef MODEL_H
#define MODEL_H

#include "glm.hpp"
#include "Core.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
};

struct Texture
{
	unsigned int id;
	std::string type;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;
};

class Model
{
public:
	Model(std::string _path);
	~Model();

	Mesh mesh;
	std::vector<Mesh> meshes;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint elementBuffer;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

private:
	bool LoadMesh(std::string _path);
	void ProcessNode(aiNode* _node, const aiScene* _scene);
	Mesh ProcessMesh(aiMesh* _mesh, const aiScene* _scene);
	void CreateVertexBuffer(Mesh _mesh);

	Assimp::Importer importer;
};
#endif
