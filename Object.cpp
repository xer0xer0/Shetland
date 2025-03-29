#include "Object.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "Camera.h"

Object::Object(std::string _meshPath, std::shared_ptr<Material> _mat)
	: material(_mat)
{
	position = glm::vec3(0.0f);
	worldMatrix = glm::mat4(1.0f);
	
	mesh = std::make_shared<Mesh>(_meshPath);
	CreateObject();
}

Object::~Object()
{
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	//glDeleteBuffers(1, &elementBuffer);
}

void Object::CreateVertexBuffer()
{
	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &elementBuffer);

	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideMult * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideMult * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Object::CreateObject()
{
	// CreateVertexBuffer();

	worldLoc = glGetUniformLocation(material->GetProgramId(), "world");
	viewLoc = glGetUniformLocation(material->GetProgramId(), "view");
	projLoc = glGetUniformLocation(material->GetProgramId(), "projection");
}

void Object::RotateWorldMatrix(float _angle, float _x, float _y, float _z)
{
	worldMatrix = glm::rotate(worldMatrix, _angle, glm::vec3(_x, _y, _z));
}

void Object::MoveWorldMatrix(float _x, float _y, float _z)
{
	worldMatrix = glm::translate(worldMatrix, glm::vec3(_x, _y, _z));
}

void Object::DrawObject()
{
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(Camera::GetInstance().GetView()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(Camera::GetInstance().GetProjection()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->GetTextureId());

	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	worldMatrix = glm::mat4(1.0f);
}