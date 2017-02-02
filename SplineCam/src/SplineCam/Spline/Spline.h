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

protected:

	void CalculateSplinePoints() {
		splineSections.clear();
		int n = controlPoints.size() - 1;
		const float step = 0.025f;

		for (int i = 0; i <= n; i++) {
			std::vector<glm::vec3> splineSection;
			for (float t = step; t < 1.0f; t += step) {
				splineSection.push_back(
					controlPoints[i - 1 > 0 ? i - 1 : 0] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
					controlPoints[i] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
					controlPoints[i + 1 < n ? i + 1 : n] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
					controlPoints[i + 2 < n ? i + 2 : n] * ((t * t * t) / 6)
				);
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
		points.push_back(controlPoints[controlPoints.size()-1]);
		return points;
	}

	std::vector<glm::vec3> controlPoints;

	std::vector<std::vector<glm::vec3>> splineSections;
};

#endif
