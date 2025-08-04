#pragma once

#include "Core.h"
#include <string>

class ComputeShader
{
public:
	ComputeShader(std::string _shaderPath);
	~ComputeShader();

	void RunShader(float _time);
	void InitOutputTexture(int _width, int _height);

	GLuint GetProgramId() { return programId; }
	GLuint GetShaderId() { return shaderId; }
	GLuint GetTextureId() { return textureId; }

private:
	void CreateProgram(std::string _shaderPath);
	GLuint InitShader(std::string _path);
	std::string ReadFromFile(std::string _path);
	bool CheckError(GLuint _shader);

	GLuint programId;
	GLuint shaderId;
	GLuint timeLoc;

	GLuint textureId;
	int textureWidth;
	int textureHeight;

	char infoLog[512];
};

