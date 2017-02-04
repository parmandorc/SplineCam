#ifndef SPLINE_EDITOR_STATE
#define SPLINE_EDITOR_STATE

#include "SplineCamState.h"

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
		spline.Init(std::vector<glm::vec3>({
			glm::vec3(0.0f, 1.0f, -15.0f),
			glm::vec3(0.0f, 1.0f, -3.0f),
			glm::vec3(-11.0f, 1.0f, -2.0f),
			glm::vec3(-11.0f, 1.0f, 3.5f),
			glm::vec3(-2.6f, 1.0f, 3.5f)
		}));
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
				spline.PreviousControlPoint();
			else
				spline.NextControlPoint();
			break;

		case GLFW_KEY_SPACE:
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				spline.DeleteControlPoint();
			else
				spline.CreateControlPoint();
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
		spline.Render(camera.ViewProjectionMatrix(), shader);
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
				spline.TranslateControlPoint(glm::vec3(x, y, z) * speed);
			}
		}
	}

	void DrawAnimatedPoint(Shader& shader)
	{
		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", camera.ViewProjectionMatrix());

		// draw the control points
		glm::vec3 point = spline.GetPoint(animationFrame);
		glm::vec3 tangent = spline.GetTangent(animationFrame);
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
	Spline spline;
};

#endif // !SPLINE_EDITOR_STATE
