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
	windowHeight(_windowHeight)
{
	light1 = std::make_shared<Light>(POINT_LIGHT);
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
	objects = std::vector<Object>();
	objects.push_back(Object(modelPath, cobbleMat, light1));
	objects.push_back(Object(modelPath, cobbleMat, light1));

	objects[1].SetPosition(glm::vec3(-1.0f, 0.5f, -1.0f));
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

	light1->InitVao();
	light1->SetPosition(glm::vec3(0.0f, 0.5f, 2.0f));
	light1->SetAmbientColor(glm::vec3(0.2f, 0.2f, 0.2f));
	light1->SetDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
	light1->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
}

void Renderer::CreateGui()
{
	ImGui::Begin("Stats");
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::End();

	// Objects
	ImGui::Begin("Objects");
	for (int i = 0; i < objects.size(); i++)
	{
		ImGui::Text("Object %i Controls", i);
		ImGui::PushID("obj" + i);
		float objPos[3] = { objects[i].GetPosition().x, objects[i].GetPosition().y, objects[i].GetPosition().z };
		if (ImGui::DragFloat3("Position##", objPos, 0.1f, -5.0f, 5.0f))
		{
			objects[i].SetPosition(glm::vec3(objPos[0], objPos[1], objPos[2]));
		}
		ImGui::PopID();
	}
	ImGui::End();

	// Lights
	ImGui::Begin("Lights");
	ImGui::PushID("light" + 1);
	float lightPos[3] = { light1->GetPosition().x, light1->GetPosition().y, light1->GetPosition().z };
	if (ImGui::DragFloat3("Position##", lightPos, 0.1f, -5.0f, 5.0f))
	{
		light1->SetPosition(glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
	}
	float lightAmbColor[3] = { light1->GetAmbientColor().r, light1->GetAmbientColor().g, light1->GetAmbientColor().b };
	if (ImGui::DragFloat3("Ambient Color##", lightAmbColor, 0.05f, 0.0f, 1.0f))
	{
		light1->SetAmbientColor(glm::vec3(lightAmbColor[0], lightAmbColor[1], lightAmbColor[2]));
	}
	float lightDiffColor[3] = { light1->GetDiffuseColor().r, light1->GetDiffuseColor().g, light1->GetDiffuseColor().b };
	if (ImGui::DragFloat3("Diffuse Color##", lightDiffColor, 0.05f, 0.0f, 1.0f))
	{
		light1->SetDiffuseColor(glm::vec3(lightDiffColor[0], lightDiffColor[1], lightDiffColor[2]));
	}
	float lightSpecColor[3] = { light1->GetSpecularColor().r, light1->GetSpecularColor().g, light1->GetSpecularColor().b };
	if (ImGui::DragFloat3("Specular Color##", lightSpecColor, 0.05f, 0.0f, 1.0f))
	{
		light1->SetSpecularColor(glm::vec3(lightSpecColor[0], lightSpecColor[1], lightSpecColor[2]));
	}
	ImGui::PopID();
	ImGui::End();
}

void Renderer::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	CreateGui();

	glUseProgram(shaderProgram->GetProgramId());

	for (int i = 0; i < objects.size(); i++)
	{
		//objects[i].RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
		objects[i].MoveWorldMatrix(objects[i].GetPosition().x, objects[i].GetPosition().y, objects[i].GetPosition().z);
		objects[i].DrawObject();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}