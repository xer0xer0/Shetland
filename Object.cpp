#include "Object.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "Camera.h"

Object::Object(std::string _meshPath, std::shared_ptr<Material> _mat)
	: material(_mat)
{
	position = glm::vec3(0.0f);
	worldMatrix = glm::mat4(1.0f);
	
	model = std::make_shared<Model>(_meshPath);
	CreateObject();
}

Object::~Object()
{
}

void Object::CreateObject()
{
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

	glBindVertexArray(model->vertexArray);
	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);

	worldMatrix = glm::mat4(1.0f);
	
}