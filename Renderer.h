#ifndef RENDERER_H
#define RENDERER_H


#include <memory>
#include <string>

#include <glm.hpp>

#include "ShaderManager.h"
#include "ComputeShader.h"
#include "Material.h"
#include "Object.h"
#include "Sky.h"
#include "Light.h"

class Renderer
{
public:
	Renderer(int _windowWidth, int _windowHeight);
	~Renderer();
	
	void InitRenderer();
	void InitImGui(GLFWwindow* _window);

	void Draw();

private:
	void InitMaterials();
	void InitLights();
	void CreateGui();
	void CreateVao();

	int windowWidth;
	int windowHeight;

	GLuint vertexArray;
	std::vector<Light> lights;
	std::vector<LightLocations> lightLocs;

	std::shared_ptr<Sky> sky;
	SkyLocations skyLocs;

	std::vector<Object> objects;
	std::shared_ptr<Object> mainObj;
	std::shared_ptr<Object> secondObj;

	std::shared_ptr<ShaderManager> shaderProgram;
	std::shared_ptr<ShaderManager> skyShaderProgram;
	std::shared_ptr<ShaderManager> irrMapShaderProgram;
	std::shared_ptr<ShaderManager> specPrefilterShaderProgram;
	std::shared_ptr<ShaderManager> brdfMapShaderProgram;
	std::shared_ptr<ComputeShader> computeShader;

	std::shared_ptr<Model> cubeModel;
	std::shared_ptr<Material> cobbleMat;
	std::shared_ptr<Material> bronzeMat;

	int vertColorPos;
	int strideMult = 8;

	int  success;
	char infoLog[512];
};

#endif
