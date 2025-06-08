#pragma once

#include "Core.h"
#include "Model.h"

#include <string>
#include <vector>

struct SkyLocations
{
	GLuint irradianceLoc;
	GLuint prefilterLoc;
	GLuint brdfLoc;
};

class Sky
{
public:
	Sky(std::shared_ptr<Model> _model, GLuint _shaderId,
		GLuint _irrMapShaderId, GLuint _specPrefilterShaderId,
		GLuint _brdfMapShaderId,
		std::string _right, std::string _left,
		std::string _top, std::string _bottom,
		std::string _front, std::string _back);

	void Draw();
	GLuint GetIrradianceMapId() { return irradianceMapId; }
	GLuint GetSpecularMapId() { return specularMapId; }
	GLuint GetPrefilterMapId() { return specPrefilterId; }
	GLuint GetBrdfMapId() { return brdfMapId; }

private:
	GLuint CreateCubemap(std::vector<std::string> _paths);
	void GenerateIrradianceMap();
	void GenerateSpecularMap();
	void GenerateBrdfMap();
	void SetShaderLocations();

	GLuint shaderId;
	GLuint cubemapId;

	GLuint supportFbo;
	GLuint supportRbo;
	GLuint irrMapShaderId;
	GLuint irradianceMapId;

	GLuint specPrefilterShaderId;
	GLuint specPrefilterId;

	GLuint brdfMapShaderId;
	GLuint brdfMapId;

	GLuint specMapShaderId;
	GLuint specularMapId;

	std::shared_ptr<Model> model;

	GLuint cameraPosLoc;
	GLuint viewLoc;
	GLuint projLoc;
	GLuint skyBoxLoc;
};

