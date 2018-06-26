#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};



class Camera
{
public:
    float YAW = -90.0f;
    float PITCH = -10.0f;
    float SPEED = 20.f;
    float SENSITIVITY = 0.3f;
    float ZOOM = 45.0f;

    glm::vec3 position;		// The positionition of the Camera in the 'world;'
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

		Camera(){};
    Camera(glm::vec3 newPos, glm::vec3 up, float newYaw,	float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
    	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
    	position = newPos;
    	worldUp = up;
    	yaw = newYaw;
    	pitch = newPitch;
    	updateCameraVectors();
    }
    Camera(glm::vec3 newPos)
    {
      position = newPos;
      worldUp = glm::vec3(0,1,0);
      yaw = YAW;
      pitch = PITCH;
      updateCameraVectors();
			movementSpeed = 20.f;
			mouseSensitivity = 0.3f;
			zoom = 45.0f;
    }

    Camera(float positionX, float positionY, float positionZ, float upX, float upY,
    	float upZ, float newYaw, float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
    	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
    	position = glm::vec3(positionX, positionY, positionZ);
    	worldUp = glm::vec3(upX, upY, upZ);
    	yaw = newYaw;
    	pitch = newPitch;
    	updateCameraVectors();
    }


    glm::mat4 getViewMatrix()
    {
    	return glm::lookAt(position, position + front, up);
    }

		glm::vec3 getPosition()
		{
			return position;
		}

    glm::vec3 processKeyBoard(Camera_Movement direction, float deltaTime)
    {
    	float velocity = movementSpeed * deltaTime;
    	if (direction == FORWARD)
    	{
    		return front * velocity;
    	}
    	if (direction == BACKWARD)
    	{
    		return -front * velocity;
    	}
    	if (direction == LEFT)
    	{
    		return -right * velocity;
    	}
    	if (direction == RIGHT)
    	{
    		return right * velocity;
    	}
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
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

    void setPosition(float x, float y, float z)
    {
    	position.x = x;
    	position.y = y;
    	position.z = z;

    	updateCameraVectors();
    }
    void processMouseScroll(float yoffset)
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

		glm::mat4 getHSRMatrix()
		{
			glm::vec3 newPos = position - front*64.0f;
			return glm::lookAt(newPos, newPos + front, up );
		}
    void updateCameraVectors()
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

};
