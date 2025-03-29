#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPE

#include "Renderer.h"

#include <iostream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

Renderer::Renderer(int _windowWidth, int _windowHeight)
	: windowWidth(_windowWidth),
	windowHeight(_windowHeight)
{
	std::cout << "init renderer" << std::endl;
}

Renderer::~Renderer()
{
	
}


void Renderer::InitRenderer()
{
	std::string vertexShaderPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Shaders/baseShader.vert";
	std::string fragmentShaderPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Shaders/baseShader.frag";
	std::string modelPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Models/cube.obj";
	shaderProgram = std::make_shared<ShaderManager>(vertexShaderPath, fragmentShaderPath);
	shaderProgram->CreateProgram();

	InitMaterials();
	mainObj = std::make_shared<Object>(modelPath, cobbleMat);
	secondObj = std::make_shared<Object>(modelPath, cobbleMat);
	secondObj->SetPosition(glm::vec3(-1.0f, 0.5f, -1.0f));
}


void Renderer::InitMaterials()
{
	cobbleMat = std::make_shared<Material>(shaderProgram);
	cobbleMat->GenerateTexture("C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Materials/cobblestone_albedo.png");

}


void Renderer::Draw()
{
	//float blue = (sin(glfwGetTime()) / 2.0f) + 0.5f;
	glUseProgram(shaderProgram->GetProgramId());

	mainObj->RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
	mainObj->MoveWorldMatrix(mainObj->GetPosition().x, mainObj->GetPosition().y, mainObj->GetPosition().z);
	mainObj->DrawObject();

	secondObj->RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
	secondObj->MoveWorldMatrix(secondObj->GetPosition().x, secondObj->GetPosition().y, secondObj->GetPosition().z);
	secondObj->DrawObject();
}