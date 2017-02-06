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

