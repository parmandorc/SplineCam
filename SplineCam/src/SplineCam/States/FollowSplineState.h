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
		camera.Init(glm::vec3(0.0f, 1.0f, -15.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 1024.0f / 768.0f, 0.1f, 1000000.0f);
	}

	void Stop() override
	{

	}

	const Camera* GetCamera() const override { return &camera; }

	void OnKeyPressed(int key) override
	{
	};

	void Update() override
	{
		camera.Update();
	}

	void Render(Shader& shader) override
	{
	}

private:

	Camera camera;
};

#endif // !FOLLOW_SPLINE_STATE_H
