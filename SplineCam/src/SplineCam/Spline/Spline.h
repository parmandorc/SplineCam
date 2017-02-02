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
		return controlPoints;
	}

protected:

	std::vector<glm::vec3> controlPoints;
};

#endif
