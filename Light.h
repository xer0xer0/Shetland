#pragma once
#include "Core.h"
#include "glm.hpp"

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
	glm::vec3 GetColor() { return color; }
	
private:
	GLuint vao;
	glm::vec3 position;
	glm::vec3 color;
};

