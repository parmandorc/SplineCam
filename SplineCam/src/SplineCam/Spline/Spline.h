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

	void Render(glm::mat4 viewProjectionMatrix, Shader shader) 
	{
		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", viewProjectionMatrix);

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
	glm::vec3 GetPoint(float t) const {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		int n = controlPoints.size() - 1;
		t *= n;
		int i = (int)t;
		return GetPoint(t - i, 
			controlPoints[i - 1 > 0 ? i - 1 : 0],
			controlPoints[i],
			controlPoints[i + 1 < n ? i + 1 : n],
			controlPoints[i + 2 < n ? i + 2 : n]);
	}
	
	glm::vec3 GetTangent(float t) {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		int n = controlPoints.size() - 1;
		t *= n;
		int i = (int)t;
		return GetTangent(t - i,
			controlPoints[i - 1 > 0 ? i - 1 : 0],
			controlPoints[i],
			controlPoints[i + 1 < n ? i + 1 : n],
			controlPoints[i + 2 < n ? i + 2 : n]);
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

	glm::vec3 GetPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) const {
		return p0 * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
			p1 * ((3 * t * t * t - 6 * t * t + 4) / 6) +
			p2 * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
			p3 * ((t * t * t) / 6);
	}

	glm::vec3 GetTangent(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		return p0 * ((-3 * t * t + 6 * t - 3) / 6) +
			p1 * ((9 * t * t - 12 * t) / 6) +
			p2 * ((-9 * t * t + 6 * t + 3) / 6) +
			p3 * ((3 * t * t) / 6);
	}

	std::vector<glm::vec3> CalculateRecursiveSubdivision(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		if (glm::distance(p1, p0 + (p3 - p0) * glm::dot(p1 - p0, p3 - p0)) < 0.05f &&
			glm::distance(p2, p0 + (p3 - p0) * glm::dot(p2 - p0, p3 - p0)) < 0.05f) {
			return std::vector<glm::vec3>();
		}

		return std::vector<glm::vec3>();
	}

	void CalculateSplinePoints() {
		splineSections.clear();
		int n = controlPoints.size() - 1;
		const float step = 0.025f;

		for (int i = 0; i <= n; i++) {
			glm::vec3 p0 = controlPoints[i - 1 > 0 ? i - 1 : 0];
			glm::vec3 p1 = controlPoints[i];
			glm::vec3 p2 = controlPoints[i + 1 < n ? i + 1 : n];
			glm::vec3 p3 = controlPoints[i + 2 < n ? i + 2 : n];

			std::vector<glm::vec3> section, result;

			section.push_back(GetPoint(0.0f, p0, p1, p2, p3));
			result = CalculateRecursiveSubdivision(p0, p1, p2, p3);
			section.insert(section.end(), result.begin(), result.end());
			section.push_back(GetPoint(1.0f, p0, p1, p2, p3));

			splineSections.push_back(section);
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
