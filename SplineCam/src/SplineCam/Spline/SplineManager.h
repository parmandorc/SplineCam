#ifndef SPLINE_MANAGER_H
#define SPLINE_MANAGER_H

#include "Spline.h"
#include <vector>

class SplineManager
{
public:
	
	static SplineManager* Get() 
	{ 
		if (!s_instance)
		{
			s_instance = new SplineManager();
		}

		return s_instance; 
	}

	void Init(int numSplines)
	{
		if (numSplines > 0)
		{
			this->numSplines = numSplines;
			splines.resize(numSplines);
		}
	}

	Spline* GetSpline(unsigned index)
	{
		Spline* spline = nullptr;
		if (index < splines.size())
		{
			spline = &splines[index];

			if (spline->ControlPoints().size() == 0) {
				// The spline is not initialized so init with some random points
				spline->Init(std::vector<glm::vec3>({
					glm::vec3(3.08f, 0.75f, -15.0f),
					glm::vec3(22.66f, 0.64f, -12.43f),
					glm::vec3(32.31f, 1.0f, -2.0f),
					glm::vec3(31.56f, 1.0f, 13.01f),
					glm::vec3(17.51f, 6.89f, 37.67f),
					glm::vec3(-3.79f, 13.13f, 44.28f),
					glm::vec3(-14.59f, 9.67f, 8.7f),
					glm::vec3(-15.83f, 5.21f, -0.69f),
					glm::vec3(-19.59f, 1.29f, -8.72f),
					glm::vec3(-9.94f, 0.18f, -17.18f),
					glm::vec3(3.08f, 0.75f, -15.0f)
				}));
			}
		}
		return spline;
	}

	~SplineManager(){}

protected:

	SplineManager()
	{
		s_instance = this;
	}

private:

	std::vector<Spline> splines;

	int numSplines = 0;
	static SplineManager* s_instance;
	
};

SplineManager* SplineManager::s_instance = nullptr;

#endif // !SPLINE_MANAGER_H

