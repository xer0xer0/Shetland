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
	InitLights();
	
	objects = std::vector<Object>();
	objects.push_back(Object(modelPath, cobbleMat, lights));
	objects.push_back(Object(modelPath, cobbleMat, lights));

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
	cobbleMat->SetTexture("Assets/Materials/cobblestone_albedo.png", ALBEDO_MAT);
	cobbleMat->SetTexture("Assets/Materials/cobblestone_normals.png", NORMAL_MAP);
	cobbleMat->SetTexture("Assets/Materials/cobblestone_roughness.png", ROUGHNESS_MAP);
	cobbleMat->SetTexture("Assets/Materials/cobblestone_metal.png", METALNESS_MAP);
}

void Renderer::InitLights()
{
	lights = std::vector<Light>();

	lights.push_back(Light(DIRECTIONAL_LIGHT));
	lights[0].InitVao();
	lights[0].SetPosition(glm::vec3(0.0f, 0.5f, 2.0f));
	lights[0].SetDirection(glm::vec3(0.0f));
	lights[0].SetAmbientColor(glm::vec3(0.2f, 0.2f, 0.2f));
	lights[0].SetDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
	lights[0].SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));

	lights.push_back(Light(POINT_LIGHT));
	lights[1].InitVao();
	lights[1].SetPosition(glm::vec3(-1.0f, 0.5f, 2.0f));
	lights[1].SetDirection(glm::vec3(0.0f));
	lights[1].SetAmbientColor(glm::vec3(0.2f, 0.2f, 0.2f));
	lights[1].SetDiffuseColor(glm::vec3(0.5f, 0.5f, 0.5f));
	lights[1].SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));

	lightLocs = std::vector<LightLocations>();
	std::string currentLight;
	for (int i = 0; i < lights.size(); i++)
	{
		currentLight = "lights[" + std::to_string(i) + "]";
		LightLocations loc;
		loc.typeLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".type").c_str());
		loc.directionLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".direction").c_str());
		loc.positionLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".position").c_str());
		loc.ambientLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".ambient").c_str());
		loc.diffuseLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".diffuse").c_str());
		loc.specularLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".specular").c_str());
		lightLocs.push_back(loc);
	}
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
		ImGui::Text("");
		ImGui::PopID();
	}
	ImGui::End();

	// Lights
	ImGui::Begin("Lights");
	for (int i = 0; i < lights.size(); i++)
	{
		ImGui::Text("Light %i Controls", i);
		ImGui::PushID("light" + i);
		float lightPos[3] = { lights[i].GetPosition().x, lights[i].GetPosition().y, lights[i].GetPosition().z};
		if (ImGui::DragFloat3("Position##", lightPos, 0.1f, -5.0f, 5.0f))
		{
			lights[i].SetPosition(glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
		}
		float lightDir[3] = { lights[i].GetDirection().x, lights[i].GetDirection().y, lights[i].GetDirection().z };
		if (ImGui::DragFloat3("Direction##", lightDir, 0.1f, -1.0f, 1.0f))
		{
			lights[i].SetDirection(glm::vec3(lightDir[0], lightDir[1], lightDir[2]));
		}
		float lightAmbColor[3] = { lights[i].GetAmbientColor().r, lights[i].GetAmbientColor().g, lights[i].GetAmbientColor().b };
		if (ImGui::DragFloat3("Ambient Color##", lightAmbColor, 0.05f, 0.0f, 1.0f))
		{
			lights[i].SetAmbientColor(glm::vec3(lightAmbColor[0], lightAmbColor[1], lightAmbColor[2]));
		}
		float lightDiffColor[3] = { lights[i].GetDiffuseColor().r, lights[i].GetDiffuseColor().g, lights[i].GetDiffuseColor().b };
		if (ImGui::DragFloat3("Diffuse Color##", lightDiffColor, 0.05f, 0.0f, 1.0f))
		{
			lights[i].SetDiffuseColor(glm::vec3(lightDiffColor[0], lightDiffColor[1], lightDiffColor[2]));
		}
		float lightSpecColor[3] = { lights[i].GetSpecularColor().r, lights[i].GetSpecularColor().g, lights[i].GetSpecularColor().b };
		if (ImGui::DragFloat3("Specular Color##", lightSpecColor, 0.05f, 0.0f, 1.0f))
		{
			lights[i].SetSpecularColor(glm::vec3(lightSpecColor[0], lightSpecColor[1], lightSpecColor[2]));
		}
		ImGui::Text("");
		ImGui::PopID();
	}
	
	ImGui::End();
}

void Renderer::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	CreateGui();

	glUseProgram(shaderProgram->GetProgramId());

	for (int i = 0; i < lights.size(); i++)
	{
		glUniform1i(lightLocs[i].typeLoc, lights[i].GetLightType());
		glUniform3f(lightLocs[i].positionLoc, lights[i].GetPosition().x, lights[i].GetPosition().y, lights[i].GetPosition().z);
		glUniform3f(lightLocs[i].directionLoc, lights[i].GetDirection().x, lights[i].GetDirection().y, lights[i].GetDirection().z);
		glUniform3f(lightLocs[i].ambientLoc, lights[i].GetAmbientColor().r, lights[i].GetAmbientColor().g, lights[i].GetAmbientColor().b);
		glUniform3f(lightLocs[i].diffuseLoc, lights[i].GetDiffuseColor().r, lights[i].GetDiffuseColor().g, lights[i].GetDiffuseColor().b);
		glUniform3f(lightLocs[i].specularLoc, lights[i].GetSpecularColor().r, lights[i].GetSpecularColor().g, lights[i].GetSpecularColor().b);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		//objects[i].RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
		objects[i].MoveWorldMatrix(objects[i].GetPosition().x, objects[i].GetPosition().y, objects[i].GetPosition().z);
		objects[i].DrawObject();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}