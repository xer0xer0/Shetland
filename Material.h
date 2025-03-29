#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "ShaderManager.h"

class Material
{
public:
	Material(std::shared_ptr<ShaderManager> _program);
	~Material();

	GLuint GetProgramId() { return shaderProgram->GetProgramId(); }
	GLuint GetTextureId() { return textureId; }
	void GenerateTexture(std::string _path);

private:
	std::shared_ptr<ShaderManager> shaderProgram;

	GLuint textureId;
	int imgWidth;
	int imgHeight;
};

#endif