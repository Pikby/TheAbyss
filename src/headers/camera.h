#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {

public:
	glm::vec3 Position;		// The position of the Camera in the 'world;'
	glm::vec3 Front;		// The direction the camera is pointing towards
	glm::vec3 Up;			// The vector perpendicular to front and points straight up
	glm::vec3 Right;		// The vector perpendicular to front and points straight right
	glm::vec3 WorldUp;		// The vector that points straight up in the world
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor
	Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	// Returns the view matri calculated using Eular Angles and the Lookat Matrix
	glm::mat4 GetViewMatrix();
	// Processes input received from any keyboard-like input system.
	void ProcessKeyBoard(Camera_Movement direction, float deltaTime);
	// Processes input received from a mouse input system
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
	// Processes input received from a mouse scroll-wheel event.
	void ProcessMouseScroll(float yoffset);

private:
	// Calculates the front vector from the Camera's updated Eular Angles
	void updateCameraVectors();
};