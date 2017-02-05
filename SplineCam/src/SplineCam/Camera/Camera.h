#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

class Camera
{

public:

	Camera() {}
	Camera(const glm::vec3& pos_, const glm::vec3& focusPos_, float fov_, float aspect_, float zNear_, float zFar_)
	{
		Init(pos_, focusPos_, fov_, aspect_, zNear_, zFar_);
	}

	virtual ~Camera() {}

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
		
	void SetFieldOfView(float fov) { this->fov = fov; }
	void SetAspect(float aspect) { this->aspect = aspect; }
	void SetZNear(float zNear) { this->zNear = zNear; }
	void SetZFar(float zFar) { this->zFar = zFar; }
	
	void OnMouseMove(float x, float y)
	{
		static glm::vec2 lastMousePos = glm::vec2(0.0f, 0.0f);
		static const float sensitivity = 0.005f;
		if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			float newAngleX = (y - lastMousePos.y) * sensitivity;
			float newAngleY = (x - lastMousePos.x) * sensitivity;

			Rotate(glm::vec3(newAngleX, newAngleY, 0.0f));
		}
			
		lastMousePos = glm::vec2(x, y);
	}

	void Update()
	{
		static const float speed = 0.01f;
		static const float angle = 0.001f;

		if (Input::isKeyPressed(GLFW_KEY_W))
		{
			Move(forward * speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_S))
		{
			Move(forward * -speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_A))
		{
			Move(right * -speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_D))
		{
			Move(right * speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_Q))
		{
			Move(up * speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_E))
		{
			Move(up * speed);
		}

		if (Input::isKeyPressed(GLFW_KEY_LEFT))
		{
			Rotate(glm::vec3(0.0f, angle, 0.0f));
		}

		if (Input::isKeyPressed(GLFW_KEY_RIGHT))
		{
			Rotate(glm::vec3(0.0f, -angle, 0.0f));
		}

		if (Input::isKeyPressed(GLFW_KEY_UP))
		{
			Rotate(glm::vec3(angle, 0.0f, 0.0f));
		}

		if (Input::isKeyPressed(GLFW_KEY_DOWN))
		{
			Rotate(glm::vec3(-angle, 0.0f, 0.0f));
		}
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

	void UpdateCameraVectors()
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