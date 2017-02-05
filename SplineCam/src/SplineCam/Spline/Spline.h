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

	void Init(const std::vector<glm::vec3>& controlPoints_, 
		float adaptiveSamplingDetailAngleThreshold_ = 0.075f,
		float adaptiveSamplingDetailDistanceThreshold_ = 1.0f)
	{
		this->controlPoints = controlPoints_;
		this->selectedControlPoint = 0;
		this->adaptiveSamplingDetailAngleThreshold = adaptiveSamplingDetailAngleThreshold_;
		this->adaptiveSamplingDetailDistanceThreshold = adaptiveSamplingDetailDistanceThreshold_;
		this->maximumSamplingDetail = 0.001f;

		CalculateSplinePoints();
	}

	void Render(glm::mat4 viewProjectionMatrix, Shader shader) 
	{
		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", viewProjectionMatrix);

		// draw the spline curve
		shader.SetUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		std::vector<glm::vec3> splinePoints = GetSplinePoints();
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < splinePoints.size() - 1; i++) {
			glVertex3f(splinePoints[i].x, splinePoints[i].y, splinePoints[i].z);
			glVertex3f(splinePoints[i + 1].x, splinePoints[i + 1].y, splinePoints[i + 1].z);
		}
		glEnd();

		// draw selected control point
		shader.SetUniform("color", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		glBegin(GL_POINTS);
		glVertex3f(controlPoints[selectedControlPoint].x, controlPoints[selectedControlPoint].y, controlPoints[selectedControlPoint].z);
		glEnd();

		// draw the control points
		shader.SetUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < controlPoints.size(); i++) {
			if (i != selectedControlPoint)
				glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
		}
		glEnd();

		// draw lines between control points
		shader.SetUniform("color", glm::vec4(0.67f, 0.67f, 0.67f, 1.0f));
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < controlPoints.size() - 1; i++) {
			glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
			glVertex3f(controlPoints[i + 1].x, controlPoints[i + 1].y, controlPoints[i + 1].z);
		}
		glEnd();
	}

	// Returns the value of the spline for the given value of the parameter t [0, 1]
	glm::vec3 GetPoint(float t) const {
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
	glm::vec3 GetPoint(float t, int i) const {
		int n = controlPoints.size() - 1;
		return controlPoints[i - 1 > 0 ? i - 1 : 0] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
			controlPoints[i] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
			controlPoints[i + 1 < n ? i + 1 : n] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
			controlPoints[i + 2 < n ? i + 2 : n] * ((t * t * t) / 6);
	}

	glm::vec3 GetTangent(float t, int i) {
		int n = controlPoints.size() - 1;
		return glm::normalize(controlPoints[i - 1 > 0 ? i - 1 : 0] * ((-3 * t * t + 6 * t - 3) / 6) +
			controlPoints[i] * ((9 * t * t - 12 * t) / 6) +
			controlPoints[i + 1 < n ? i + 1 : n] * ((-9 * t * t + 6 * t + 3) / 6) +
			controlPoints[i + 2 < n ? i + 2 : n] * ((3 * t * t) / 6));
	}

	std::vector<glm::vec3> CalculateRecursiveSubdivision(int i, float t0, float t1, glm::vec3 x0, glm::vec3 x1, glm::vec3 m0, glm::vec3 m1) {
		if (t1 - t0 < maximumSamplingDetail) { // Avoid infinite recursion
			return std::vector<glm::vec3>();
		}

		float t = (t0 + t1) * 0.5f;
		glm::vec3 x = GetPoint(t, i);
		glm::vec3 m = GetTangent(t, i);

		// Curve is consider a line when the tangent in the middle point is practically the same as the tangents in both extremes
		// This works because only one inflection point maximum will exist inside a bspline section.
		// Hence, if the middle point's tangent is aligned with its extreme, it has to be a line. 
		// If it was a more complex curve, there would have to be more than one inflection point.
		if (glm::acos(glm::clamp(glm::dot(m0, m), -1.0f, 1.0f)) < adaptiveSamplingDetailAngleThreshold &&
				glm::acos(glm::clamp(glm::dot(m1, m), -1.0f, 1.0f)) < adaptiveSamplingDetailAngleThreshold) {

			// Extra text for long almost straight lines
			if (glm::distance(x, x0 + (x1 - x0) * glm::dot(x - x0, x1 - x0)) < adaptiveSamplingDetailDistanceThreshold) {
				return std::vector<glm::vec3>();
			}
		}

		std::vector<glm::vec3> result, pre, post;
		pre = CalculateRecursiveSubdivision(i, t0, t, x0, x, m0, m);
		post = CalculateRecursiveSubdivision(i, t, t1, x, x1, m, m1);
		result.insert(result.end(), pre.begin(), pre.end());
		result.push_back(GetPoint(t, i));
		result.insert(result.end(), post.begin(), post.end());
		return result;
	}

	void CalculateSplinePoints() {
		splineSections.clear();
		int n = controlPoints.size() - 1;
		const float step = 0.025f;

		for (int i = 0; i <= n; i++) {
			std::vector<glm::vec3> section, result;

			glm::vec3 x0 = GetPoint(0.0f, i);
			glm::vec3 x1 = GetPoint(1.0f, i);
			section.push_back(x0);
			result = CalculateRecursiveSubdivision(i, 0.0f, 1.0f, x0, x1, GetTangent(0.0f, i), GetTangent(1.0f, i));
			section.insert(section.end(), result.begin(), result.end());
			section.push_back(x1);
			splineSections.push_back(section);

			printf("%d: %d\n", i, section.size());
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

	// Index to the currently selected control point.
	// Transformations will be performed to this point.
	unsigned int selectedControlPoint;

	// This parameter controls the level of detail of the curve in adaptive sampling,
	//	in the maximum angle difference allowed between tangents
	float adaptiveSamplingDetailAngleThreshold;

	// This parameter controls the level of detail of the curve in adaptive sampling,
	//	in the maximum distance allowed between a point and the curve.
	float adaptiveSamplingDetailDistanceThreshold;

	// This parameter controls the maximum depth of the recursion in the adaptive sampling.
	// This shouldn't be used to control quality of the curve.
	// It is recommended to keep its default value, unless the program crashes for memory allocation issues.
	float maximumSamplingDetail;
};

#endif
