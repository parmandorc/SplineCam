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
		if (spline->ControlPoints().size() == 0)
		{
			// The spline is not initialized so init with some random points
			spline->Init(std::vector<glm::vec3>({
				glm::vec3(3.08f, 0.75f, -15.0f),
				glm::vec3(22.66f, 0.64f, -12.43f),
				glm::vec3(32.31f, 1.0f, -2.0f),
				glm::vec3(31.56f, 1.0f, 13.01f),
				glm::vec3(17.51f, 6.89f, 37.67f),
				glm::vec3(-3.79f, 13.13f, 44.28f),
				glm::vec3(-14.59f, 9.67f, 8.7f),
				glm::vec3(-15.83f, 5.21f, -0.69f),
				glm::vec3(-19.59f, 1.29f, -8.72f),
				glm::vec3(-9.94f, 0.18f, -17.18f),
				glm::vec3(3.08f, 0.75f, -15.0f)
			}),
				std::vector<glm::vec3>({
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
				glm::vec3(),
			}));
		}
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

		case GLFW_KEY_SPACE:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				spline->DeleteControlPoint();
			else
				spline->CreateControlPoint();
			break;

		case GLFW_KEY_BACKSPACE:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				spline->DeleteCustomOrientation();
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

		animationFrame = fmodf(animationFrame + 0.00025f, 1);
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
			int y = Input::isKeyPressed(GLFW_KEY_W) - Input::isKeyPressed(GLFW_KEY_S);
			int z = Input::isKeyPressed(GLFW_KEY_Q) - Input::isKeyPressed(GLFW_KEY_E);
			if (x != 0 || y != 0 || z != 0) 
			{
				spline->TranslateControlPoint(glm::vec3(x, y, z) * speed);
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

	// camera
	FreeCamera camera;

	// spline
	Spline* spline = nullptr;
};

#endif // !SPLINE_EDITOR_STATE
