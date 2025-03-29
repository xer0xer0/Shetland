#pragma once

#include <iostream>

// GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Camera.h"

class Main
{
public:
	Main(int _windowWidth, int _windowHeight);
	~Main();
	bool InitWindow();
	void WaitForInput();

private:
	GLFWwindow* window;
	int windowWidth;
	int windowHeight;

	std::unique_ptr<Renderer> renderer;
};

int main()
{
	Main main(800, 600);
	if (!main.InitWindow())
	{
		return 1;
	}
	main.WaitForInput();

	return 0;
}

