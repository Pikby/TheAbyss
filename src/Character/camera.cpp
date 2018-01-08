#include <iostream>
#include "../headers/camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float newYaw,	float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = position;
	worldUp = up;
	yaw = newYaw;
	pitch = newPitch;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
	float upZ, float newYaw, float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	yaw = newYaw;
	pitch = newPitch;
	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getHSRMatrix()
{
	glm::vec3 newPos = position - front*64.0f;
	return glm::lookAt(newPos, newPos + front, up );
}

void Camera::processKeyBoard(Camera_Movement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
	{
		position += front * velocity;
	}
	if (direction == BACKWARD)
	{
		position -= front * velocity;
	}
	if (direction == LEFT)
	{
		position -= right * velocity;
	}
	if (direction == RIGHT)
	{
		position += right * velocity;
	}
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure when pitch is out of bounds, the screen doesn't flip
	if (constrainPitch)
	{
		if (pitch > 89.0f)
		{
			pitch = 89.0;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}
	updateCameraVectors();
}

void Camera::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	updateCameraVectors();
}
void Camera::processMouseScroll(float yoffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
	{
		zoom -= yoffset;
	}
	if (zoom <= 1.0f)
	{
		zoom = 1.0f;
	}
	if (zoom >= 45.0f)
	{
		zoom = 45.0f;
	}
}

void Camera::updateCameraVectors()
{
	// calculate a new front vector
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);
	// also recalculate the other 2 vectors
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
