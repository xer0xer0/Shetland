#include "Sky.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Assets/Includes/stb_image.h"
#include <iostream>

Sky::Sky(std::shared_ptr<Model> _model, GLuint _shaderId, GLuint _irrMapShaderId,
	std::string _right, std::string _left, std::string _top, std::string _bottom, std::string _front, std::string _back)
	: model(_model),
	shaderId(_shaderId),
	irrMapShaderId(_irrMapShaderId)
{
	std::vector<std::string> paths = { _right, _left, _top, _bottom, _front, _back };
	cubemapId = CreateCubemap(paths);
	GenerateIrradianceMap();
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

void Sky::GenerateIrradianceMap()
{
	glGenTextures(1, &irradianceMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapId);
	
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 irrMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 irrMapViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glGenFramebuffers(1, &irrMapFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, irrMapFbo);

	glGenRenderbuffers(1, &irrMapRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, irrMapRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, irrMapRbo);

	viewLoc = glGetUniformLocation(irrMapShaderId, "view");
	projLoc = glGetUniformLocation(irrMapShaderId, "projection");
	skyBoxLoc = glGetUniformLocation(irrMapShaderId, "skybox");

	GLint currentViewport[4];
	glGetIntegerv(GL_VIEWPORT, currentViewport);
	glViewport(0, 0, 32, 32);

	glUseProgram(irrMapShaderId);
	glDepthFunc(GL_LEQUAL);
	glUniform1i(skyBoxLoc, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(irrMapProjection));

	for (int i = 0; i < 6; i++)
	{
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(irrMapViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMapId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);
	}
	glDepthFunc(GL_LESS);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(currentViewport[0], currentViewport[1], currentViewport[2], currentViewport[3]);
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