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
		int n = controlPoints.size() - 1;
		float step = 0.025f;
		points.push_back(controlPoints[0]);
		for (float u = step; u < 1.0f; u += step) {
			float t = u * n;
			int m = (int)t;
			t = t - m;
			glm::vec3 p = controlPoints[m-1 > 0 ? m-1 : 0] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
					controlPoints[m] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
					controlPoints[m+1 < n ? m+1 : n] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
					controlPoints[m+2 < n ? m+2 : n] * ((t * t * t) / 6);
			points.push_back(p);
		}
		points.push_back(controlPoints[n]);
		return points;
	}

	std::vector<glm::vec3> getControlPoints() {
		return std::vector<glm::vec3>(controlPoints);
	}

protected:

	std::vector<glm::vec3> controlPoints;
};

#endif
