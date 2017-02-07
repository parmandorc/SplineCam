#ifndef FOLLOW_SPLINE_STATE_H
#define FOLLOW_SPLINE_STATE_H

#include "SplineCamState.h"

class FollowSplineState : public SplineCamState
{
public:
	FollowSplineState() {};
	~FollowSplineState() {};

	void Start() override
	{
		// init camera
		spline = SplineManager::Get()->GetSpline(0);
		if (spline->ControlPoints().size() == 0)
		{
			// The spline is not initialized so init with some random points
			spline->Init(std::vector<glm::vec3>({
				glm::vec3(0.0f, 1.0f, -15.0f),
				glm::vec3(0.0f, 1.0f, -3.0f),
				glm::vec3(-11.0f, 1.0f, -2.0f),
				glm::vec3(-11.0f, 1.0f, 3.5f),
				glm::vec3(-2.6f, 1.0f, 3.5f)
			}),
			std::vector<glm::vec3>({
				glm::vec3(),
				glm::vec3(1.0f, 0.5f, 1.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec3(0.0f, -0.25f, 1.0f),
				glm::vec3()
			}));
		}

		camera.Init(spline, 45.0f, 1024.0f / 768.0f, 0.1f, 1000000.0f);
	}

	void Stop() override
	{

	}

	const Camera* GetCamera() const override { return &camera; }

	void OnKeyPressed(int key) override
	{
		switch (key)
		{

		case GLFW_KEY_ENTER:
			doRenderSpline = !doRenderSpline;
			break;

		case GLFW_KEY_F2:
			if (doRenderSpline)
				spline->ToggleDebugPoints();
			break;
		}
	};

	void Update() override
	{
		camera.Update();
	}

	void Render(Shader& shader) override
	{
		if (doRenderSpline)
			spline->Render(camera.ViewProjectionMatrix(), shader);
	}

private:

	FollowSplineCamera camera;

	Spline* spline;

	bool doRenderSpline;
};

#endif // !FOLLOW_SPLINE_STATE_H
