#include "Light.h"

Light::Light()
{
	color = glm::vec3(0);
}

Light::~Light()
{

}

void Light::InitVao()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 3 because model takes position, normal, and tex coord indices
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Light::SetColor(float _r, float _g, float _b)
{
	color = glm::vec3(_r, _g, _b);
}