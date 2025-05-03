#include "Material.h"
#include "STB.h"

#include <iostream>

Material::Material(std::shared_ptr<ShaderManager> _program)
	: shaderProgram(_program)
{

}

Material::~Material()
{

}

void Material::SetTexture(std::string _path, TextureTypes _type)
{
	switch (_type)
	{
		case ALBEDO_MAT:
			albedoTextureId = GenerateTexture(_path, GL_RGB);
			break;
		case NORMAL_MAP:
			normalMapId = GenerateTexture(_path, GL_RGB);
			break;
		case ROUGHNESS_MAP:
			roughnessMapId = GenerateTexture(_path, GL_R);
			break;
		case METALNESS_MAP:
			metalnessMapId = GenerateTexture(_path, GL_R);
			break;
	}
}

GLuint Material::GenerateTexture(std::string _path, GLenum _colorFormat)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int numColorChannels;
	unsigned char* imgData = stbi_load(_path.c_str(), &imgWidth, &imgHeight, &numColorChannels, 0);
	if (imgData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, _colorFormat, GL_UNSIGNED_BYTE, imgData);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Loaded texture at " << _path << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture at " << _path << std::endl;
	}

	stbi_image_free(imgData);

	return id;
}