#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "Core.h"

#include <string>

class ShaderManager
{
public:
	ShaderManager(std::string _vertexPath, std::string _fragmentPath);
	~ShaderManager();

	GLuint GetProgramId() { return programId; }
	GLuint GetVertexShaderId() { return vertexId; }
	GLuint GetFragmentShaderId() { return fragmentId; }

private:
	void CreateProgram(std::string _vertexPath, std::string _fragmentPath);
	GLuint InitShader(GLenum _type, std::string _path);
	std::string ReadFromFile(std::string _path);
	bool CheckError(GLuint _shader);

	GLuint programId;
	GLuint vertexId;
	GLuint fragmentId;

	char infoLog[512];
};

#endif