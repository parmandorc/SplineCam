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

		camera.Init(spline, 45.0f, 1024.0f / 768.0f, 0.1f, 1000000.0f);

		doRenderSpline = false;
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

		case GLFW_KEY_SPACE:
			camera.isPaused = !camera.isPaused;
			break;
		}
	};

	void Update() override
	{
		camera.doRewind = Input::isKeyPressed(GLFW_KEY_Z);
		camera.doFastForward = Input::isKeyPressed(GLFW_KEY_X);
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
