#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "ShaderManager.h"

struct MatLocations
{
	GLuint albedoLoc;
	GLuint normalLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
	GLuint shininessLoc;
};

class Material
{
public:
	Material(std::shared_ptr<ShaderManager> _program);
	~Material();

	GLuint GetProgramId() { return shaderProgram->GetProgramId(); }
	GLuint GetAlbedoTextureId() { return albedoTextureId; }
	GLuint GetNormalMapId() { return normalMapId; }
	void SetAlbedoTexture(std::string _path);
	void SetNormalMap(std::string _path);

private:
	GLuint GenerateTexture(std::string _path);

	std::shared_ptr<ShaderManager> shaderProgram;

	GLuint albedoTextureId;
	GLuint normalMapId;
	int imgWidth;
	int imgHeight;
};

#endif