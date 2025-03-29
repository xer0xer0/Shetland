#include "Main.h"

void MouseCallback(GLFWwindow* _window, double _mousePosX, double _mousePosY);

Main::Main(int _windowWidth, int _windowHeight):
	windowWidth(_windowWidth),
	windowHeight(_windowHeight)
{
	std::cout << "Start" << std::endl;
	renderer = std::make_unique<Renderer>(windowWidth, windowHeight);
}

Main::~Main()
{
	glfwTerminate();
}

bool Main::InitWindow()
{
	if (!glfwInit())
	{
		std::cout << "GLFW failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Test", NULL, NULL);

	if (!window)
	{
		std::cout << "Window failed" << std::endl;
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, MouseCallback);
	
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed" << std::endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	Camera::GetInstance().InitValues(windowWidth, windowHeight);
	renderer->InitRenderer();
	return true;
}

void MouseCallback(GLFWwindow* _window, double _mousePosX, double _mousePosY)
{
	float x = static_cast<float>(_mousePosX);
	float y = static_cast<float>(_mousePosY);
	Camera::GetInstance().ProcessMouseMovement(x, y);
}

void Main::WaitForInput()
{
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Camera::GetInstance().ProcessKeyboardInput(window);
		renderer->Draw();

		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
		&& glfwWindowShouldClose(window) == 0);
}