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
		for (float u = 0.0f; u <= 1.0f; u += 0.025f) {
			float t = u * n;
			int m = (int)t;
			glm::vec3 p;
			if (m == 0) {
				t = t / 3;
				p = controlPoints[0] * (-t * t * t + 3 * t * t - 3 * t + 1) +
					controlPoints[1] * (3 * t * t * t - 6 * t * t + 3 * t) +
					controlPoints[2] * (-3 * t * t * t + 3 * t * t) +
					controlPoints[3] * (t * t * t);
			}
			else if (m >= n - 1) {
				t = (t - m + 2) / 3;
				p =	controlPoints[n-3] * (-t * t * t + 3 * t * t - 3 * t + 1) +
					controlPoints[n-2] * (3 * t * t * t - 6 * t * t + 3 * t) +
					controlPoints[n-1] * (-3 * t * t * t + 3 * t * t) +
					controlPoints[n] * (t * t * t);
			}
			else {
				t = t - m;
				p = controlPoints[m-1] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
					controlPoints[m] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
					controlPoints[m+1] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
					controlPoints[m+2] * ((t * t * t) / 6);
			}
			points.push_back(p);
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
