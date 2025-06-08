#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "ShaderManager.h"

struct MatLocations
{
	GLuint albedoLoc;
	GLuint normalLoc;
	GLuint roughnessLoc;
	GLuint metalnessLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
	GLuint shininessLoc;
};

enum TextureTypes
{
	ALBEDO_MAT,
	NORMAL_MAP,
	ROUGHNESS_MAP,
	METALNESS_MAP
};

class Material
{
public:
	Material(std::shared_ptr<ShaderManager> _program);
	~Material();

	GLuint GetProgramId() { return shaderProgram->GetProgramId(); }
	GLuint GetAlbedoTextureId() { return albedoTextureId; }
	GLuint GetNormalMapId() { return normalMapId; }
	GLuint GetRoughnessMapId() { return roughnessMapId; }
	GLuint GetMetalnessMapId() { return metalnessMapId; }

	void SetTexture(std::string _path, TextureTypes _type, GLenum _format = GL_RED);

private:
	GLuint GenerateTexture(std::string _path, GLenum _colorFormat);

	std::shared_ptr<ShaderManager> shaderProgram;

	GLuint albedoTextureId;
	GLuint normalMapId;
	GLuint roughnessMapId;
	GLuint metalnessMapId;
	int imgWidth;
	int imgHeight;
};

#endif