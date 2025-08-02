#include "ShaderManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

ShaderManager::ShaderManager(std::string _vertexPath, std::string _fragmentPath)
{
	std::cout << "Creating shader program:" << std::endl;
	CreateProgram(_vertexPath, _fragmentPath);
}

ShaderManager::~ShaderManager()
{
	glDeleteProgram(programId);
}

bool ShaderManager::CheckError(GLuint _shader)
{
	int success;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void ShaderManager::CreateProgram(std::string _vertexPath, std::string _fragmentPath)
{
	// Vertex shader
	vertexId = InitShader(GL_VERTEX_SHADER, _vertexPath);

	// Fragment shader
	fragmentId = InitShader(GL_FRAGMENT_SHADER, _fragmentPath);

	// Shader program
	programId = glCreateProgram();
	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	glLinkProgram(programId);

	int success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Clean up shaders (already linked)
	glDetachShader(programId, vertexId);
	glDeleteShader(vertexId);
	glDetachShader(programId, fragmentId);
	glDeleteShader(fragmentId);
}

GLuint ShaderManager::InitShader(GLenum _type, std::string _path)
{
	std::string shaderText = ReadFromFile(_path);
	const char* source = shaderText.c_str();

	GLuint shaderId = glCreateShader(_type);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	CheckError(shaderId);
	std::cout << " Loaded shader " << _path << std::endl;
	return shaderId;
}

std::string ShaderManager::ReadFromFile(std::string _path)
{
	std::cout << " Loading shader " << _path << std::endl;
	std::string outText;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(_path);
		std::stringstream fileStream;
		fileStream << file.rdbuf();

		file.close();
		outText = fileStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	return outText;
}