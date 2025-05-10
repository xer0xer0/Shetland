#pragma once
#include "Core.h"
#include "glm.hpp"

struct LightLocations
{
	GLuint typeLoc;
	GLuint positionLoc;
	GLuint directionLoc;
	GLuint colorLoc;
	GLuint diffuseLoc;
	GLuint specularLoc;
};

enum LightTypes
{
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	SPOT_LIGHT
};

class Light
{
public:
	Light(LightTypes _type);
	~Light();

	glm::vec3 GetPosition() { return position; }
	void SetPosition(glm::vec3 _position) { position = _position; }

	glm::vec3 GetDirection() { return direction; }
	void SetDirection(glm::vec3 _direction) { direction = _direction; }

	void InitVao();
	void SetColor(glm::vec3 _color) { color = _color; }
	void SetColor(float _r, float _g, float _b);

	GLuint GetLightVao() { return vao; }
	int GetLightType() { return type; }
	glm::vec3 GetColor() { return color; }
	
private:
	GLuint vao;

	int type;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
};

