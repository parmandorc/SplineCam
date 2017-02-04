#ifndef FOLLOW_SPLINE_CAMERA_H
#define FOLLOW_SPLINE_CAMERA_H

#include "Camera.h"
#include "../Spline/Spline.h"

class FollowSplineCamera : public Camera
{
public:
	FollowSplineCamera() {}
	~FollowSplineCamera() {}

	void Update() override
	{
		if (spline)
		{
			glm::vec3 nextPoint = spline->GetPoint(t);

			Move(nextPoint - pos);

			t = fmodf(t + 0.00025f, 1);
		}
	}

	void SetSpline(const Spline* spline)
	{
		this->spline = spline;
	}

private:

	// spline
	const Spline* spline;

	// animatedPoint
	float t = 0.0f;
};

#endif // !FOLLOW_SPLINE_CAMERA_H

