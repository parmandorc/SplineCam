#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

class Spline
{

public:

	Spline() {}
	Spline(std::vector<glm::vec3>& controlPoints_)
		: controlPoints(controlPoints_)
	{
	}

	virtual ~Spline() {}

	void Init(std::vector<glm::vec3>& controlPoints_)
	{
		this->controlPoints = controlPoints_;
		for (glm::vec3 c : this->controlPoints) {
			printf("(%f, %f, %f)\n", c.x, c.y, c.z);
		}
	}

	std::vector<glm::vec3> getSplinePoints() {
		std::vector<glm::vec3> points;
		for (float t = 0.0f; t <= 1.0f; t += 0.025f) {
			points.push_back(
				controlPoints[0] * (-t * t * t + 3 * t * t - 3 * t + 1) +
				controlPoints[1] * (3 * t * t * t - 6 * t * t + 3 * t) +
				controlPoints[2] * (-3 * t * t * t + 3 * t * t) +
				controlPoints[3] * (t * t * t)
			);
		}
		return points;
	}

	std::vector<glm::vec3> getControlPoints() {
		return std::vector<glm::vec3>(controlPoints);
	}

protected:

	std::vector<glm::vec3> controlPoints;
};

#endif
