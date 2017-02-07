#ifndef FOLLOW_SPLINE_CAMERA_H
#define FOLLOW_SPLINE_CAMERA_H

#include "Camera.h"
#include "../Spline/Spline.h"

class FollowSplineCamera : public Camera
{
public:
	FollowSplineCamera() {}
	~FollowSplineCamera() {}

	void Init(const Spline* spline, float fov, float aspect, float zNear, float zFar)
	{
		this->spline = spline;
		Camera::Init(spline->GetPoint(t), glm::normalize(spline->GetTangent(t)), fov, aspect, zNear, zFar);
	}

	void Update() override
	{
		if (spline)
		{
			glm::vec3 nextPoint = spline->GetPoint(t);
			
			Move(nextPoint - pos);

			if (!isPaused || doRewind || doFastForward) {
				int step = doRewind ? -1 : !isPaused + doFastForward;
				t += 0.00015f * step;
				if (t < 0)
					t += (int)t + 1;
				t = fmodf(t, 1);
			}
		}
	}

	bool isPaused = false, doRewind = false, doFastForward = false;


private:

	void UpdateCameraVectors() override
	{
		// set the forward vector according to the tangent
		glm::vec3 tangent = spline->GetTangent(t);
		forward = glm::normalize(tangent);

		// set the right vector by crossing the forward with the Y axis
		right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));

		// set the up vector by crossing the forward and right vector
		up = glm::cross(right, forward);

		// set the new focus position
		focusPos = pos + forward;
	}

private:

	// spline
	const Spline* spline;

	// animatedPoint
	float t = 0.0f;
};

#endif // !FOLLOW_SPLINE_CAMERA_H

