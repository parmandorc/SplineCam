#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include "Camera.h"

class FPSCamera : public Camera
{
public:
	FPSCamera() {};
	~FPSCamera() {};

	void OnMouseMove(float x, float y) override
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

	void Update() override
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
	}

private:

};

#endif // !FPS_CAMERA_H

