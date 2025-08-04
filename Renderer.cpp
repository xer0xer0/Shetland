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
	std::cout << "Initalizing Renderer..." << std::endl;
}

Renderer::~Renderer()
{
	glDeleteVertexArrays(1, &vertexArray);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


void Renderer::InitRenderer()
{
	CreateVao();

	std::string vertexShaderPath = "Assets/Shaders/baseShader.vert";
	std::string fragmentShaderPath = "Assets/Shaders/baseShader.frag";
	shaderProgram = std::make_shared<ShaderManager>(vertexShaderPath, fragmentShaderPath);

	skyShaderProgram = std::make_shared<ShaderManager>("Assets/Shaders/skyShader.vert", "Assets/Shaders/skyShader.frag");
	irrMapShaderProgram = std::make_shared<ShaderManager>("Assets/Shaders/cubemap.vert", "Assets/Shaders/irradianceMap.frag");
	specPrefilterShaderProgram = std::make_shared<ShaderManager>("Assets/Shaders/cubemap.vert", "Assets/Shaders/prefilterMap.frag");
	brdfMapShaderProgram = std::make_shared<ShaderManager>("Assets/Shaders/brdfMap.vert", "Assets/Shaders/brdfMap.frag");
	computeShader = std::make_shared<ComputeShader>("Assets/Shaders/computeBase.comp");

	cubeModel = std::make_shared<Model>("Assets/Models/cube.obj", vertexArray);

	sky = std::make_shared<Sky>(cubeModel, skyShaderProgram->GetProgramId(),
		irrMapShaderProgram->GetProgramId(), specPrefilterShaderProgram->GetProgramId(),
		brdfMapShaderProgram->GetProgramId(),
		"Assets/Skyboxes/Clouds Blue/right.png", "Assets/Skyboxes/Clouds Blue/left.png",
		"Assets/Skyboxes/Clouds Blue/up.png", "Assets/Skyboxes/Clouds Blue/down.png",
		"Assets/Skyboxes/Clouds Blue/front.png", "Assets/Skyboxes/Clouds Blue/back.png");
	skyLocs.irradianceLoc = glGetUniformLocation(shaderProgram->GetProgramId(), "irradianceMap");
	skyLocs.prefilterLoc = glGetUniformLocation(shaderProgram->GetProgramId(), "prefilterMap");
	skyLocs.brdfLoc = glGetUniformLocation(shaderProgram->GetProgramId(), "brdfMap");

	InitMaterials();
	InitLights();
	
	objects = std::vector<Object>();
	objects.push_back(Object(cubeModel, cobbleMat, lights));
	objects.push_back(Object(cubeModel, bronzeMat, lights));

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
	cobbleMat->SetTexture("Assets/Materials/cobblestone_albedo.png", ALBEDO_MAT, GL_RGB); // bit depth = 24
	cobbleMat->SetTexture("Assets/Materials/cobblestone_normals.png", NORMAL_MAP, GL_RGB);
//	cobbleMat->SetTexture("Assets/Materials/cobblestone_roughness.png", ROUGHNESS_MAP);
	cobbleMat->SetTexture(computeShader->GetTextureId(), ROUGHNESS_MAP);
	cobbleMat->SetTexture("Assets/Materials/cobblestone_metal.png", METALNESS_MAP);

	bronzeMat = std::make_shared<Material>(shaderProgram);
	bronzeMat->SetTexture("Assets/Materials/bronze_albedo.png", ALBEDO_MAT, GL_RGBA); // bit depth = 32
	bronzeMat->SetTexture("Assets/Materials/bronze_normals.png", NORMAL_MAP, GL_RGBA);
	bronzeMat->SetTexture("Assets/Materials/bronze_roughness.png", ROUGHNESS_MAP);
	bronzeMat->SetTexture("Assets/Materials/bronze_metal.png", METALNESS_MAP);
}

void Renderer::InitLights()
{
	lights = std::vector<Light>();

	lights.push_back(Light(DIRECTIONAL_LIGHT));
	lights[0].InitVao();
	lights[0].SetPosition(glm::vec3(0.0f, 0.5f, 2.0f));
	lights[0].SetDirection(glm::vec3(0.0f, 0.0f, -0.3f));
	lights[0].SetColor(glm::vec3(0.2f, 0.2f, 0.2f));

	lights.push_back(Light(POINT_LIGHT));
	lights[1].InitVao();
	lights[1].SetPosition(glm::vec3(-1.0f, 0.5f, 2.0f));
	lights[1].SetDirection(glm::vec3(0.0f));
	lights[1].SetColor(glm::vec3(0.2f, 0.2f, 0.2f));

	lightLocs = std::vector<LightLocations>();
	std::string currentLight;
	for (int i = 0; i < lights.size(); i++)
	{
		currentLight = "lights[" + std::to_string(i) + "]";
		LightLocations loc;
		loc.typeLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".type").c_str());
		loc.directionLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".direction").c_str());
		loc.positionLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".position").c_str());
		loc.colorLoc = glGetUniformLocation(shaderProgram->GetProgramId(), (currentLight + ".color").c_str());
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
		float lightAmbColor[3] = { lights[i].GetColor().r, lights[i].GetColor().g, lights[i].GetColor().b };
		if (ImGui::DragFloat3("Color##", lightAmbColor, 0.05f, 0.0f, 1.0f))
		{
			lights[i].SetColor(lightAmbColor[0], lightAmbColor[1], lightAmbColor[2]);
		}
		ImGui::Text("");
		ImGui::PopID();
	}
	
	ImGui::End();
}

void Renderer::CreateVao()
{
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	// tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
}

void Renderer::Draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	CreateGui();

	glBindVertexArray(vertexArray);

	computeShader->RunShader(glfwGetTime());

	glUseProgram(shaderProgram->GetProgramId());
	
	glUniform1i(skyLocs.irradianceLoc, 0);
	glUniform1i(skyLocs.prefilterLoc, 1);
	glUniform1i(skyLocs.brdfLoc, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky->GetIrradianceMapId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, sky->GetPrefilterMapId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sky->GetBrdfMapId());

	for (int i = 0; i < lights.size(); i++)
	{
		glUniform1i(lightLocs[i].typeLoc, lights[i].GetLightType());
		glUniform3f(lightLocs[i].positionLoc, lights[i].GetPosition().x, lights[i].GetPosition().y, lights[i].GetPosition().z);
		glUniform3f(lightLocs[i].directionLoc, lights[i].GetDirection().x, lights[i].GetDirection().y, lights[i].GetDirection().z);
		glUniform3f(lightLocs[i].colorLoc, lights[i].GetColor().r, lights[i].GetColor().g, lights[i].GetColor().b);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		// objects[i].RotateWorldMatrix((float)glfwGetTime() * glm::radians(10.0f), 0.5f, 1.0f, 0.0f);
		objects[i].MoveWorldMatrix(objects[i].GetPosition().x, objects[i].GetPosition().y, objects[i].GetPosition().z);
		objects[i].DrawObject();
	}

	glUseProgram(skyShaderProgram->GetProgramId());
	sky->Draw();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}