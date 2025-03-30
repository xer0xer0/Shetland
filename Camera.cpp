#include "Camera.h"

#include <iostream>

Camera* Camera::instance = nullptr;
Camera::Camera()
{
	
}

Camera::~Camera()
{

}

void Camera::InitValues(int _windowWidth, int _windowHeight)
{
	mouseReceived = false;
	pitch = 0.0f;
	yaw = -90.0f;

	position = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	forward = glm::vec3(0.0f, 0.0f, -1.0f);

	projection = glm::perspective(glm::radians(45.0f), (float)_windowWidth / (float)_windowHeight, 0.1f, 100.0f);

	UpdateVectors();
}

void Camera::SetView()
{
	view = glm::lookAt(position, position + forward, up);
}

void Camera::UpdateVectors()
{
	// positive z-axis
	glm::vec3 forwardTemp;
	forwardTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forwardTemp.y = sin(glm::radians(pitch));
	forwardTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(forwardTemp);

	// positrive x-axis
	right = glm::normalize(glm::cross(forward, up));

	// positive y-axis
	up = glm::normalize(glm::cross(right, forward));
}

void Camera::ProcessKeyboardInput(GLFWwindow* _window)
{
	if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
		position += forward * speed;
	if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forward * speed;
	if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * speed;
	if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * speed;
	if (glfwGetKey(_window, GLFW_KEY_F12) == GLFW_PRESS)
		std::cout << "Pressed" << std::endl;

	SetView();
}

void Camera::ProcessMouseMovement(float _mousePosX, float _mousePosY)
{
	if (!mouseReceived)
	{
		lastMousePosX = _mousePosX;
		lastMousePosY = _mousePosY;
		mouseReceived = true;
	}

	float xOffset = _mousePosX - lastMousePosX;
	float yOffset = lastMousePosY - _mousePosY;
	lastMousePosX = _mousePosX;
	lastMousePosY = _mousePosY;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// clamp
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	UpdateVectors();
}