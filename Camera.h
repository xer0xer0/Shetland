#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	static Camera& GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new Camera();
		}
		return *instance;
	}

	Camera(Camera const& _camera) = delete;
	void operator=(Camera const& _camera) = delete;

	~Camera();

	void InitValues(int _windowWidth, int _windowHeight);
	void ProcessKeyboardInput(GLFWwindow* _window);
	void ProcessMouseMovement(float _mousePosX, float _mousePosY);

	glm::mat4 GetView() { return view; }
	glm::mat4 GetProjection() { return projection; }
	glm::vec3 GetPosition() { return position; }

protected:
	Camera();
	static Camera* instance;

private:
	void SetView();
	void UpdateVectors();

	const glm::vec3 worldSpaceUp = glm::vec3(0.0f, 1.0f, 0.0f);
	const float speed = 0.05f;
	const float sensitivity = 0.1f;

	glm::mat4 view;
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;

	float pitch; // y-axis
	float yaw; // x-axis;

	bool mouseReceived;
	float lastMousePosX;
	float lastMousePosY;

	glm::mat4 projection;
};

