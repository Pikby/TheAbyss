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
const float PITCH = -10.0f;
const float SPEED = 20.f;
const float SENSITIVITY = 0.3f;
const float ZOOM = 45.0f;

class Camera {

public:
	glm::vec3 position;		// The position of the Camera in the 'world;'
	glm::vec3 front;		// The direction the camera is pointing towards
	glm::vec3 up;			// The vector perpendicular to front and points straight up
	glm::vec3 right;		// The vector perpendicular to front and points straight right
	glm::vec3 worldUp;		// The vector that points straight up in the world
	// Eular Angles
	float yaw;
	float pitch;
	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	// constructor
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	// Returns the view matri calculated using Eular Angles and the Lookat Matrix
	glm::mat4 getViewMatrix();
	// Processes input received from any keyboard-like input system.
	void processKeyBoard(Camera_Movement direction, float deltaTime);
	// Processes input received from a mouse input system
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
	// Processes input received from a mouse scroll-wheel event.
	void processMouseScroll(float yoffset);

	void setPosition(float x, float y, float z);
private:
	// Calculates the front vector from the Camera's updated Eular Angles
	void updateCameraVectors();
};
