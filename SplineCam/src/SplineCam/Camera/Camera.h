#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

class Camera
{
public:
	virtual ~Camera() {}
	virtual void OnMouseMove(float x, float y){}
	virtual void Update(){}

	void Init(const glm::vec3& pos, const glm::vec3& focusPos, float fov, float aspect, float zNear, float zFar)
	{
		this->pos = pos;
		this->focusPos = focusPos;
		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;

		UpdateCameraVectors();
	}
	
	glm::mat4 ViewProjectionMatrix() const
	{
		glm::mat4 view = glm::mat4();
		
		view = glm::lookAt(pos, focusPos, up);

		glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
		return projection * view;
	}

	void Move(const glm::vec3& offset)
	{
		pos += offset;

		UpdateCameraVectors();
	}
	void Rotate(const glm::vec3& rot)
	{
		xAngle += rot.x;
		yAngle += rot.y;

		UpdateCameraVectors();
	}

protected:
	Camera() {}

	virtual void UpdateCameraVectors()
	{
		// set the forward vector according to xAngle and yAngle(spherical to cartesian coordinates)
		forward.x = cosf(xAngle) * sinf(yAngle);
		forward.y = sinf(xAngle);
		forward.z = cosf(xAngle) * cosf(yAngle);

		forward = glm::normalize(forward);

		// set the right vector by crossing the forward with the Y axis
		right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));

		// set the up vector by crossing the forward and right vector
		up = glm::cross(right, forward);

		// set the new focus position
		focusPos = pos + forward;
	}

	float fov = 45.0f; // in degrees
	float aspect = 1.33f;
	float zNear = 0.1f;
	float zFar = 1000.0f;

	glm::vec3 pos;
	glm::vec3 focusPos;

	// camera up, forward, right vectors
	glm::vec3 up;
	glm::vec3 forward;
	glm::vec3 right;

	// rotation by euler angles
	float xAngle = 0.0f;
	float yAngle = 0.0f;
};

#endif