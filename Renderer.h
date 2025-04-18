#ifndef RENDERER_H
#define RENDERER_H


#include <memory>
#include <string>

#include <glm.hpp>

#include "ShaderManager.h"
#include "Material.h"
#include "Object.h"
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
	void CreateGui();

	int windowWidth;
	int windowHeight;

	Light light1;

	std::shared_ptr<Object> mainObj;
	std::shared_ptr<Object> secondObj;

	std::shared_ptr<ShaderManager> shaderProgram;

	std::shared_ptr<Material> cobbleMat;

	int vertColorPos;
	int strideMult = 8;

	int  success;
	char infoLog[512];
};

#endif
