#pragma once
#include "Core.h"
#include "glm.hpp"

struct LightLocations
{
	GLuint positionLoc;
	GLuint ambientLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
};

class Light
{
public:
	Light();
	~Light();

	glm::vec3 GetPosition() { return position; }
	void SetPosition(glm::vec3 _position) { position = _position; }

	void InitVao();
	void SetColor(float _r, float _g, float _b);

	GLuint GetLightVao() { return vao; }
	glm::vec3 GetAmbientColor() { return ambientColor; }
	glm::vec3 GetDiffuseColor() { return diffuseColor; }
	glm::vec3 GetSpecularColor() { return specularColor; }

	void SetAmbientColor(glm::vec3 _color) { ambientColor = _color; }
	void SetDiffuseColor(glm::vec3 _color) { diffuseColor = _color; }
	void SetSpecularColor(glm::vec3 _color) { specularColor = _color; }
	
private:
	GLuint vao;
	glm::vec3 position;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
};

