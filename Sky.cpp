#include "Sky.h"
#include "glm.hpp"
#include "Camera.h"
#include "Assets/Includes/stb_image.h"
#include <iostream>

Sky::Sky(std::shared_ptr<Model> _model, GLuint _shaderId,
	std::string _right, std::string _left, std::string _top, std::string _bottom, std::string _front, std::string _back)
	: model(_model),
	shaderId(_shaderId)
{
	std::vector<std::string> paths = { _right, _left, _top, _bottom, _front, _back };
	cubemapId = CreateCubemap(paths);
	SetShaderLocations();
}

GLuint Sky::CreateCubemap(std::vector<std::string> _paths)
{
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width;
	int height;
	int numColorChannels;

	for (int i = 0; i < _paths.size(); i++)
	{
		unsigned char* imgData = stbi_load(_paths[i].c_str(), &width, &height, &numColorChannels, 0);

		if (imgData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
			std::cout << "Loaded skybox texture at " << _paths[i] << std::endl;
		}
		else
		{
			std::cout << "Failed to load skybox texture at " << _paths[i] << std::endl;
		}

		stbi_image_free(imgData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return id;
}

void Sky::SetShaderLocations()
{
	cameraPosLoc = glGetUniformLocation(shaderId, "cameraPos");
	viewLoc = glGetUniformLocation(shaderId, "view");
	projLoc = glGetUniformLocation(shaderId, "projection");
}

void Sky::Draw()
{
	glDepthFunc(GL_LEQUAL);
	
	glm::mat4 view = glm::mat4(glm::mat3((Camera::GetInstance().GetView())));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(Camera::GetInstance().GetProjection()));
	glUniform3f(cameraPosLoc, Camera::GetInstance().GetPosition().x, Camera::GetInstance().GetPosition().y, Camera::GetInstance().GetPosition().z);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);

	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);
	glDepthFunc(GL_LESS);
}