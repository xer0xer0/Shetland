#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPE

#include "Renderer.h"

#include <iostream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Assets/Includes/DearImGui/imgui.h"
#include "Assets/Includes/DearImGui/imgui_impl_glfw.h"
#include "Assets/Includes/DearImGui/imgui_impl_opengl3.h"

Renderer::Renderer(int _windowWidth, int _windowHeight)
	: windowWidth(_windowWidth),
	windowHeight(_windowHeight),
	light1(POINT_LIGHT)
{
	std::cout << "init renderer" << std::endl;
}

Renderer::~Renderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void Renderer::InitRenderer()
{
	std::string vertexShaderPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Shaders/baseShader.vert";
	std::string fragmentShaderPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Shaders/baseShader.frag";
	std::string modelPath = "C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Models/cube.obj";
	shaderProgram = std::make_shared<ShaderManager>(vertexShaderPath, fragmentShaderPath);
	shaderProgram->CreateProgram();

	InitMaterials();
	mainObj = std::make_shared<Object>(modelPath, cobbleMat, light1);
	secondObj = std::make_shared<Object>(modelPath, cobbleMat, light1);
	secondObj->SetPosition(glm::vec3(-1.0f, 0.5f, -1.0f));
}

void Renderer::InitImGui(GLFWwindow* _window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Renderer::InitMaterials()
{
	cobbleMat = std::make_shared<Material>(shaderProgram);
	cobbleMat->GenerateTexture("C:/Graphics Libs/Projects/Shetland/Shetland/Assets/Materials/cobblestone_albedo.png");

	light1.InitVao();
	light1.SetPosition(glm::vec3(0.0f, 0.5f, 2.0f));
	light1.SetAmbientColor(glm::vec3(0.2f, 0.2f, 0.2f));
	light1.SetDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
	light1.SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
}

void Renderer::CreateGui()
{
	ImGui::Begin("Test Window");
	ImGui::Text("This is some text!");
	ImGui::End();
}

void Renderer::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	CreateGui();

	glUseProgram(shaderProgram->GetProgramId());

	mainObj->RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
	mainObj->MoveWorldMatrix(mainObj->GetPosition().x, mainObj->GetPosition().y, mainObj->GetPosition().z);
	mainObj->DrawObject();

	secondObj->RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
	secondObj->MoveWorldMatrix(secondObj->GetPosition().x, secondObj->GetPosition().y, secondObj->GetPosition().z);
	secondObj->DrawObject();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}