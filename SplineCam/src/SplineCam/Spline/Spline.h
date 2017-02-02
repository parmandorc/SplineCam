#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

class Spline
{

public:

	Spline() {}
	Spline(const std::vector<glm::vec3>& controlPoints_)
		: controlPoints(controlPoints_)
	{
	}

	virtual ~Spline() {}

	void Init(const std::vector<glm::vec3>& controlPoints_)
	{
		this->controlPoints = controlPoints_;
		CalculateSplinePoints();
	}

	void Render(glm::mat4 viewProjectionMatrix, Shader shader) {
		// build modelViewProjection matrix
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)) * glm::rotate(model, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelViewProjection = viewProjectionMatrix * model;

		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", modelViewProjection);

		// draw the spline curve
		std::vector<glm::vec3> splinePoints = GetSplinePoints();
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < splinePoints.size() - 1; i++) {
			glVertex3f(splinePoints[i].x, splinePoints[i].y, splinePoints[i].z);
			glVertex3f(splinePoints[i + 1].x, splinePoints[i + 1].y, splinePoints[i + 1].z);
		}
		glEnd();

		// draw the control points
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		for (glm::vec3 c : controlPoints) {
			glVertex3f(c.x, c.y, c.z);
		}
		glEnd();
	}

	// Returns the value of the spline for the given value of the parameter t [0, 1]
	glm::vec3 GetPoint(float t) {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		t *= controlPoints.size() - 1;
		int i = (int)t;
		return GetPoint(t - i, i);
	}
	
	glm::vec3 GetTangent(float t) {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		t *= controlPoints.size() - 1;
		int i = (int)t;
		return GetTangent(t - i, i);
	}

	void NextControlPoint() { selectedControlPoint = (selectedControlPoint + 1) % controlPoints.size(); }
	void PreviousControlPoint() { selectedControlPoint = selectedControlPoint == 0 ? controlPoints.size() - 1 : selectedControlPoint - 1; }
	void TranslateControlPoint(glm::vec3 translate) { controlPoints[selectedControlPoint] += translate; CalculateSplinePoints(); }

	void CreateControlPoint() { 
		controlPoints.insert(controlPoints.begin() + selectedControlPoint, controlPoints[selectedControlPoint]);
		if (selectedControlPoint != controlPoints.size())
			NextControlPoint();
		CalculateSplinePoints();
	}

	void DeleteControlPoint() { 
		if (controlPoints.size() > 1) {
			controlPoints.erase(controlPoints.begin() + selectedControlPoint);
			if (selectedControlPoint != 0)
				PreviousControlPoint();
			CalculateSplinePoints();
		}
	}

protected:

	// Calculates the value of the i-th spline section for the given value of the parameter t [0, 1]
	glm::vec3 GetPoint(float t, int i) {
		int n = controlPoints.size() - 1;
		return controlPoints[i - 1 > 0 ? i - 1 : 0] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
			controlPoints[i] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
			controlPoints[i + 1 < n ? i + 1 : n] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
			controlPoints[i + 2 < n ? i + 2 : n] * ((t * t * t) / 6);
	}

	glm::vec3 GetTangent(float t, int i) {
		int n = controlPoints.size() - 1;
		return controlPoints[i - 1 > 0 ? i - 1 : 0] * ((-3 * t * t + 6 * t - 3) / 6) +
			controlPoints[i] * ((9 * t * t - 12 * t) / 6) +
			controlPoints[i + 1 < n ? i + 1 : n] * ((-9 * t * t + 6 * t + 3) / 6) +
			controlPoints[i + 2 < n ? i + 2 : n] * ((3 * t * t) / 6);
	}

	void CalculateSplinePoints() {
		splineSections.clear();
		int n = controlPoints.size() - 1;
		const float step = 0.025f;

		for (int i = 0; i <= n; i++) {
			std::vector<glm::vec3> splineSection;
			for (float t = step; t < 1.0f; t += step) {
				splineSection.push_back(GetPoint(t, i));
			}
			splineSections.push_back(splineSection);
		}
	}

	std::vector<glm::vec3> GetSplinePoints() {
		std::vector<glm::vec3> points;
		points.push_back(controlPoints[0]);
		for (std::vector<glm::vec3> section : splineSections) {
			points.insert(points.end(), section.begin(), section.end());
		}
		points.push_back(controlPoints[controlPoints.size() - 1]);
		return points;
	}

	// The control points that define the spline
	std::vector<glm::vec3> controlPoints;

	// The computed points that draw the spline
	std::vector<std::vector<glm::vec3>> splineSections;

	unsigned int selectedControlPoint = 0;
};

#endif
