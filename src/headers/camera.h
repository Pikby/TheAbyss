#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
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

		glm::mat4 projection;
    // Eular Angles
    float yaw;
    float pitch;
    // Camera options
    float movementSpeed;
    float mouseSensitivity;

		float zoomInDegrees=45.0f;
		float near = 0.1f;
		float far = 100.0;
		float ar = 1;

		Camera(){};
    Camera(glm::vec3 newPos, glm::vec3 up, float newYaw,	float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
    	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomInDegrees(ZOOM)
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
			zoomInDegrees= 45.0f;
    }

    Camera(float positionX, float positionY, float positionZ, float upX, float upY,
    	float upZ, float newYaw, float newPitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
    	movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoomInDegrees(ZOOM)
    {
    	position = glm::vec3(positionX, positionY, positionZ);
    	worldUp = glm::vec3(upX, upY, upZ);
    	yaw = newYaw;
    	pitch = newPitch;
    	updateCameraVectors();
    }
		void updateViewProjection()
		{
			projection = glm::perspective(glm::radians(zoomInDegrees),ar,near,far);
		}
		void setAspectRatio(float Ar)
		{
			ar = Ar;
			updateViewProjection();
		}

		void setCameraZoom(float cameraZoomInDegrees)
		{
			zoomInDegrees = cameraZoomInDegrees;
			updateViewProjection();
		}

		void setCameraNearFar(float camNear, float camFar)
		{
			near = camNear;
			far = camFar;
			updateViewProjection();
		}


    glm::mat4 getViewMatrix() const
    {
    	return glm::lookAt(glm::vec3(0), glm::vec3(0) + front, up);
    }

		glm::mat4 getProjectionMatrix()
		{
			updateViewProjection();
			return projection;
		}

		glm::vec3 getPosition() const
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

		void calculateFrustrum(glm::vec3* arr)
		{
			float buffer = 0;
		  float fovH = glm::radians(((zoomInDegrees+buffer)/2)*ar);
		  float fovV = glm::radians((zoomInDegrees+buffer)/2);
		  glm::vec3 curPos = position - front*((float)32*2);
		  glm::vec3 rightaxis = glm::rotate(front,fovH,up);
		  glm::vec3 toprightaxis = glm::rotate(rightaxis,fovV,right);
		  glm::vec3 bottomrightaxis = glm::rotate(rightaxis,-fovV,right);
		  glm::vec3 leftaxis = glm::rotate(front,-fovH,up);
		  glm::vec3 topleftaxis = glm::rotate(leftaxis,fovV,right);
		  glm::vec3 bottomleftaxis = glm::rotate(leftaxis,-fovV,right);


		  //std::cout << "nnear and far" << near << ":" << far <<  "\n";
		  float d;
		  float a = near;
		  d = a/(glm::dot(bottomleftaxis,front));
		  arr[0] = d*bottomleftaxis+curPos;
		  d = a/(glm::dot(topleftaxis,front));
		  arr[1] = d*topleftaxis+curPos;
		  d = a/(glm::dot(toprightaxis,front));
		  arr[2] =  d*toprightaxis+curPos;
		  d = a/(glm::dot(bottomrightaxis,front));
		  arr[3] = d*bottomrightaxis+curPos;


		  a = far;
		  d = a/(glm::dot(bottomleftaxis,front));
		  arr[4] = d*bottomleftaxis+curPos;
		  d = a/(glm::dot(topleftaxis,front));
		  arr[5] = d*topleftaxis+curPos;
		  d = a/(glm::dot(toprightaxis,front));
		  arr[6] =  d*toprightaxis+curPos;
		  d = a/(glm::dot(bottomrightaxis,front));
		  arr[7] = d*bottomrightaxis+curPos;

		}


    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
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

    void setPosition(const glm::vec3& pos)
    {
    	position = pos;

    	updateCameraVectors();
    }
    void processMouseScroll(float yoffset)
    {
    	if (zoomInDegrees >= 1.0f && zoomInDegrees <= 45.0f)
    	{
    		zoomInDegrees -= yoffset;
    	}
    	if (zoomInDegrees <= 1.0f)
    	{
    		zoomInDegrees= 1.0f;
    	}
    	if (zoomInDegrees>= 45.0f)
    	{
    		zoomInDegrees = 45.0f;
    	}
    }

		glm::mat4 getHSRMatrix() const
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
