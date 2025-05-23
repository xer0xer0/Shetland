#pragma once

#include "Core.h"
#include "Model.h"

#include <string>
#include <vector>

class Sky
{
public:
	Sky(std::shared_ptr<Model> _model, GLuint _shaderId, GLuint _irrMapShaderId,
		std::string _right, std::string _left,
		std::string _top, std::string _bottom,
		std::string _front, std::string _back);

	void Draw();
	GLuint GetIrradianceMapId() { return irradianceMapId; }


private:
	GLuint CreateCubemap(std::vector<std::string> _paths);
	void GenerateIrradianceMap();
	void SetShaderLocations();

	GLuint shaderId;
	GLuint cubemapId;

	GLuint irrMapShaderId;
	GLuint irradianceMapId;
	GLuint irrMapFbo;
	GLuint irrMapRbo;

	std::shared_ptr<Model> model;

	GLuint cameraPosLoc;
	GLuint viewLoc;
	GLuint projLoc;
	GLuint skyBoxLoc;
};

