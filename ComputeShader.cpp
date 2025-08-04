#include "ComputeShader.h"

#include <iostream>
#include <fstream>
#include <sstream>

ComputeShader::ComputeShader(std::string _shaderPath)
	: textureId(-1),
	textureWidth(1024),
	textureHeight(1024)
{
	CreateProgram(_shaderPath);
	InitOutputTexture(textureWidth, textureHeight);
}

ComputeShader::~ComputeShader()
{
	glDeleteTextures(1, &textureId);
	glDeleteProgram(programId);
}

bool ComputeShader::CheckError(GLuint _shader)
{
	int success;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(_shader, 512, NULL, infoLog);
		std::cout << "ERROR::COMPUTE_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void ComputeShader::RunShader(float _time)
{
	if (textureId < 0)
	{
		throw std::exception("Error running compute shader - no texture created.");
	}

	// std::cout << _time << std::endl;
	glUseProgram(programId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1f(timeLoc, _time);
	glDispatchCompute((GLuint)textureWidth / 8, (GLuint)textureHeight / 8, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::InitOutputTexture(int _width, int _height)
{
	textureWidth = _width;
	textureHeight = _height;

	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, textureWidth, textureHeight, 0, GL_RGBA,
		GL_FLOAT, NULL);

	glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void ComputeShader::CreateProgram(std::string _shaderPath)
{
	shaderId = InitShader(_shaderPath);

	// Shader program
	programId = glCreateProgram();
	glAttachShader(programId, shaderId);
	glLinkProgram(programId);

	int success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	timeLoc = glGetUniformLocation(programId, "time");

	// Clean up shaders (already linked)
	glDetachShader(programId, shaderId);
	glDeleteShader(shaderId);
}

GLuint ComputeShader::InitShader(std::string _path)
{
	std::string shaderText = ReadFromFile(_path);
	const char* source = shaderText.c_str();

	GLuint shaderId = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);

	CheckError(shaderId);
	return shaderId;
}

std::string ComputeShader::ReadFromFile(std::string _path)
{
	std::cout << _path << std::endl;
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

