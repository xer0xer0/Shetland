#ifndef RENDERER_H
#define RENDERER_H


#include <memory>
#include <string>

#include <glm.hpp>

#include "ShaderManager.h"
#include "Material.h"
#include "Object.h"

class Renderer
{
public:
	Renderer(int _windowWidth, int _windowHeight);
	~Renderer();
	
	void InitRenderer();
	
	void Draw();

private:
	void InitMaterials();

	int windowWidth;
	int windowHeight;

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
