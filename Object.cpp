#include "Object.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "Camera.h"

Object::Object(std::shared_ptr<Model> _model, std::shared_ptr<Material> _mat, std::vector<Light> _lights)
	: model(_model),
	material(_mat),
	lights(_lights)
{
	position = glm::vec3(0.0f);
	worldMatrix = glm::mat4(1.0f);

	CreateObject();
}

Object::~Object()
{
}

void Object::CreateObject()
{
	worldLoc = glGetUniformLocation(material->GetProgramId(), "world");
	worldInvTransPos = glGetUniformLocation(material->GetProgramId(), "worldInvTranspose");
	viewLoc = glGetUniformLocation(material->GetProgramId(), "view");
	projLoc = glGetUniformLocation(material->GetProgramId(), "projection");
	cameraPosLoc = glGetUniformLocation(material->GetProgramId(), "cameraPos");

	matLocs.albedoLoc = glGetUniformLocation(material->GetProgramId(), "albedoTex");
	matLocs.normalLoc = glGetUniformLocation(material->GetProgramId(), "normalMap");
	matLocs.roughnessLoc = glGetUniformLocation(material->GetProgramId(), "roughnessMap");
	matLocs.metalnessLoc = glGetUniformLocation(material->GetProgramId(), "metalnessMap");
	matLocs.diffuseLoc = glGetUniformLocation(material->GetProgramId(), "material.diffuse");
	matLocs.specularLoc = glGetUniformLocation(material->GetProgramId(), "material.specular");
	matLocs.shininessLoc = glGetUniformLocation(material->GetProgramId(), "material.shininess");
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
	glm::mat3 worldInvTrans = glm::transpose(glm::inverse(glm::mat3(worldMatrix)));
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix3fv(worldInvTransPos, 1, GL_FALSE, glm::value_ptr(worldInvTrans));
	glUniformMatrix4fv(viewLoc,  1, GL_FALSE, glm::value_ptr(Camera::GetInstance().GetView()));
	glUniformMatrix4fv(projLoc,  1, GL_FALSE, glm::value_ptr(Camera::GetInstance().GetProjection()));
	glUniform3f(cameraPosLoc, Camera::GetInstance().GetPosition().x, Camera::GetInstance().GetPosition().y, Camera::GetInstance().GetPosition().z);

	glUniform1i(matLocs.albedoLoc, 0);
	glUniform1i(matLocs.normalLoc, 1);
	glUniform1i(matLocs.roughnessLoc, 2);
	glUniform1i(matLocs.metalnessLoc, 3);
	glUniform1i(matLocs.diffuseLoc, 0);
	glUniform3f(matLocs.specularLoc, 0.5f, 0.5f, 0.5f);
	glUniform1f(matLocs.shininessLoc, 32.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->GetAlbedoTextureId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->GetNormalMapId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, material->GetRoughnessMapId());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, material->GetMetalnessMapId());

	glDrawElements(GL_TRIANGLES, model->indices.size(), GL_UNSIGNED_INT, 0);
	
	worldMatrix = glm::mat4(1.0f);
	
}