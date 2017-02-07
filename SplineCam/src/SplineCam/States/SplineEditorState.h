#ifndef SPLINE_EDITOR_STATE
#define SPLINE_EDITOR_STATE

#include "SplineCamState.h"
#include "../Spline/SplineManager.h"

class SplineEditorState : public SplineCamState
{
public:
	SplineEditorState() {};
	~SplineEditorState() {};

	void Start() override
	{
		// init camera
		camera.Init(glm::vec3(0.0f, 1.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 1024.0f / 768.0f, 0.1f, 1000000.0f);

		// init spline
		spline = SplineManager::Get()->GetSpline(0);

		isPaused = false;
	}

	void Stop() override
	{

	}

	const Camera* GetCamera() const override { return &camera; }

	void OnKeyPressed(int key) override
	{
		switch (key)
		{
		case GLFW_KEY_TAB:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				spline->PreviousControlPoint();
			else
				spline->NextControlPoint();
			break;

		case GLFW_KEY_ENTER:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				animationFrame = spline->CreateControlPoint(animationFrame); //from camera
			else
				animationFrame = spline->CreateControlPoint(animationFrame);
			break;

		case GLFW_KEY_BACKSPACE:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				spline->DeleteCustomOrientation();
			else
				animationFrame = spline->DeleteControlPoint(animationFrame);
			break;

		case GLFW_KEY_SPACE:
			isPaused = !isPaused;
			break;

		case GLFW_KEY_F2:
			spline->ToggleDebugPoints();
			break;

		case GLFW_KEY_F3:
			spline->PrintControlPoints();
			break;
		}

	};
	void Update() override
	{
		camera.Update();
		UpdateSpline();

		if (!isPaused || Input::isKeyPressed(GLFW_KEY_Z) || Input::isKeyPressed(GLFW_KEY_X)) {
			int step = Input::isKeyPressed(GLFW_KEY_Z) ? -1 : !isPaused + Input::isKeyPressed(GLFW_KEY_X);
			animationFrame += 0.00025f * step;
			if (animationFrame < 0)
				animationFrame += (int)animationFrame + 1;
			animationFrame = fmodf(animationFrame, 1);
		}
	}

	void Render(Shader& shader) override
	{
		spline->Render(camera.ViewProjectionMatrix(), shader);
		DrawAnimatedPoint(shader);
	}

protected:

	void UpdateSpline() 
	{
		static const float speed = 0.01f;
		if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) 
		{
			int x = Input::isKeyPressed(GLFW_KEY_D) - Input::isKeyPressed(GLFW_KEY_A);
			int y = Input::isKeyPressed(GLFW_KEY_Q) - Input::isKeyPressed(GLFW_KEY_E);
			int z = Input::isKeyPressed(GLFW_KEY_W) - Input::isKeyPressed(GLFW_KEY_S);
			if (x != 0 || y != 0 || z != 0) 
			{
				glm::mat3 axis = camera.GetAxis();
				axis[2].y = 0.0f;
				axis[2] = glm::normalize(axis[2]);
				axis[1] = glm::vec3(0.0f, 1.0f, 0.0f);
				axis[0] = glm::cross(axis[2], axis[1]);
				spline->TranslateControlPoint((axis[0] * (float)x + axis[1] * (float)y + axis[2] * (float)z) * speed);
			}

			int ax = Input::isKeyPressed(GLFW_KEY_UP) - Input::isKeyPressed(GLFW_KEY_DOWN);
			int ay = Input::isKeyPressed(GLFW_KEY_LEFT) - Input::isKeyPressed(GLFW_KEY_RIGHT);
			if (ax != 0 || ay != 0)
			{
				spline->RotateControlPoint(ax * speed, ay * speed);
			}
		}
	}

	void DrawAnimatedPoint(Shader& shader)
	{
		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", camera.ViewProjectionMatrix());
		shader.SetUniform("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		// draw the control points
		glm::vec3 point = spline->GetPoint(animationFrame);
		glm::vec3 tangent = spline->GetTangent(animationFrame);
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3f(point.x, point.y, point.z);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(point.x, point.y, point.z);
		glVertex3f(point.x + tangent.x, point.y + tangent.y, point.z + tangent.z);
		glEnd();
	}

private:

	// animatedPoint
	float animationFrame = 0.0f;
	bool isPaused;

	// camera
	FreeCamera camera;

	// spline
	Spline* spline = nullptr;
};

#endif // !SPLINE_EDITOR_STATE
