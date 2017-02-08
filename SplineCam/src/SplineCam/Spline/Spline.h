#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

class Spline
{

public:

	Spline() {}
	Spline(const std::vector<glm::vec3>& controlPoints_, const std::vector<glm::vec3>& orientations_)
		: controlPoints(controlPoints_)
		, orientations(orientations_)
	{
	}

	virtual ~Spline() {}

	void Init(const std::vector<glm::vec3>& controlPoints_,
		const std::vector<glm::vec3>& orientations_ = std::vector<glm::vec3>(),
		const bool isCyclic_ = false,
		const float adaptiveSamplingDetailAngleThreshold_ = 0.075f,
		const float adaptiveSamplingDetailDistanceThreshold_ = 1.0f)
	{
		this->controlPoints = controlPoints_;
		this->orientations = orientations_;
		if (this->orientations.size() != this->controlPoints.size())
			this->orientations = std::vector<glm::vec3>(this->controlPoints.size());
		this->selectedControlPoint = 0;
		this->adaptiveSamplingDetailAngleThreshold = adaptiveSamplingDetailAngleThreshold_;
		this->adaptiveSamplingDetailDistanceThreshold = adaptiveSamplingDetailDistanceThreshold_;
		this->maximumSamplingDetail = 0.001f;
		this->drawDebugPoints = false;
		this->isCyclic = isCyclic_;

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

		// draw selected control point custom orientation
		shader.SetUniform("color", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		glBegin(GL_LINES);
		glVertex3f(controlPoints[selectedControlPoint].x, controlPoints[selectedControlPoint].y, controlPoints[selectedControlPoint].z);
		glVertex3f(controlPoints[selectedControlPoint].x + orientations[selectedControlPoint].x, 
			controlPoints[selectedControlPoint].y + orientations[selectedControlPoint].y, 
			controlPoints[selectedControlPoint].z + orientations[selectedControlPoint].z);
		glEnd();

		// draw the control points
		shader.SetUniform("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < controlPoints.size(); i++) {
			if (i != selectedControlPoint)
				glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
		}
		glEnd();

		// draw custom orientations
		shader.SetUniform("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < orientations.size(); i++) {
			if (i != selectedControlPoint) {
				glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
				glVertex3f(controlPoints[i].x + orientations[i].x, controlPoints[i].y + orientations[i].y, controlPoints[i].z + orientations[i].z);
			}
		}
		glEnd();

		// draw lines between control points
		shader.SetUniform("color", glm::vec4(0.67f, 0.67f, 0.67f, 1.0f));
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < controlPoints.size() - 1; i++) {
			glVertex3f(controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
			glVertex3f(controlPoints[i + 1].x, controlPoints[i + 1].y, controlPoints[i + 1].z);
		}
		if (isCyclic) {
			glVertex3f(controlPoints[controlPoints.size() - 1].x, controlPoints[controlPoints.size() - 1].y, controlPoints[controlPoints.size() - 1].z);
			glVertex3f(controlPoints[0].x, controlPoints[0].y, controlPoints[0].z);
		}
		glEnd();

		if (drawDebugPoints) {
			// draw the control points
			shader.SetUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			for (unsigned int i = 0; i < splinePoints.size(); i++) {
				glVertex3f(splinePoints[i].x, splinePoints[i].y, splinePoints[i].z);
			}
			glEnd();
		}
	}

	// Returns the value of the spline for the given value of the parameter t [0, 1]
	glm::vec3 GetPoint(float t) const {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		t *= controlPoints.size() - !isCyclic;
		int i = (int)t;
		return GetPoint(t - i, i);
	}
	
	glm::vec3 GetTangent(float t) const {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		t *= controlPoints.size() - !isCyclic;
		int i = (int)t;
		return GetOrientation(t - i, i);
	}

	void NextControlPoint() { selectedControlPoint = (selectedControlPoint + 1) % controlPoints.size(); }
	void PreviousControlPoint() { selectedControlPoint = selectedControlPoint == 0 ? controlPoints.size() - 1 : selectedControlPoint - 1; }
	void TranslateControlPoint(glm::vec3 translate) { controlPoints[selectedControlPoint] += translate; CalculateSplinePoints(); }
	void RotateControlPoint(float dx, float dy) {
		if (orientations[selectedControlPoint] == glm::vec3())
			orientations[selectedControlPoint] = glm::vec3(1.0f, 0.0f, 0.0f);

		glm::mat4 mat(1);
		mat = glm::rotate(mat, dy, glm::vec3(0.0f, 1.0f, 0.0f));
		mat = glm::rotate(mat, dx, glm::cross(orientations[selectedControlPoint], glm::vec3(0.0f, 1.0f, 0.0f)));
		orientations[selectedControlPoint] = glm::normalize(glm::vec3(mat * glm::vec4(orientations[selectedControlPoint], 1.0f)));
	}

	float CreateControlPoint(float t, glm::vec3 position = glm::vec3(), glm::vec3 orientation = glm::vec3()) {
		t = t < 0 ? 0 : t > 1 ? 1 : t;
		t *= controlPoints.size() - 1;
		int i = (int)t;
		if (position == glm::vec3()) {
			position = GetPoint(t - i, i);
		}

		controlPoints.insert(controlPoints.begin() + i + 1, position);
		orientations.insert(orientations.begin() + i + 1, orientation);

		selectedControlPoint = i + 1;
		CalculateSplinePoints();

		return (float)(i + 1) / (controlPoints.size() - 1);
	}

	float DeleteControlPoint(float t) { 
		if (controlPoints.size() > 2) {
			t = t < 0 ? 0 : t > 1 ? 1 : t;
			t *= controlPoints.size() - 1;
			int i = (int)t;

			controlPoints.erase(controlPoints.begin() + selectedControlPoint);
			orientations.erase(orientations.begin() + selectedControlPoint);

			float newT = 0.0f;
			if (i != 0 || selectedControlPoint != 0) {
				newT = t - 1;
				if (i == selectedControlPoint) {
					newT += (1 - (t - i)) * 0.5f;
				}
			}

			if (selectedControlPoint != 0)
				PreviousControlPoint();
			CalculateSplinePoints();

			return newT / (controlPoints.size() - 1);
		}

		return t;
	}

	void DeleteCustomOrientation() { orientations[selectedControlPoint] = glm::vec3(); }

	void ToggleDebugPoints() { drawDebugPoints = !drawDebugPoints; }

	const std::vector<glm::vec3>& ControlPoints() const { return controlPoints; }
	const glm::vec3& SelectedControlPoint() const { return controlPoints[selectedControlPoint]; }

	void PrintControlPoints()
	{
		for (auto& point : controlPoints)
		{
			printf("( %f, %f, %f)\n", point.x, point.y, point.z);
		}
	}

	void ToggleCyclicOrClamped() { isCyclic = !isCyclic; CalculateSplinePoints(); }

protected:

	int GetIndex(int i) const {
		int n = controlPoints.size() - !isCyclic;
		if (!isCyclic)
			return i < 0 ? 0 : (i < n ? i : n);
		else
			return i < 0 ? (i % -n) + n : i % n;
	}

	// Calculates the value of the i-th spline section for the given value of the parameter t [0, 1]
	glm::vec3 GetPoint(float t, int i) const {
		int n = controlPoints.size() - 1;
		return controlPoints[GetIndex(i - 1)] * ((-t * t * t + 3 * t * t - 3 * t + 1) / 6) +
			controlPoints[GetIndex(i)] * ((3 * t * t * t - 6 * t * t + 4) / 6) +
			controlPoints[GetIndex(i + 1)] * ((-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6) +
			controlPoints[GetIndex(i + 2)] * ((t * t * t) / 6);
	}

	glm::vec3 GetOrientation(float t, int i) const {
		int n = controlPoints.size() - 1;
		glm::vec3 tangent = GetTangent(t, i);
		glm::vec3 a = tangent, b = tangent;
		if (orientations[GetIndex(i)] != glm::vec3()) {
			a = orientations[GetIndex(i)];
		}
		if (orientations[GetIndex(i + 1)] != glm::vec3()) {
			b = orientations[GetIndex(i + 1)];
		}
		t = (1 - cosf(float(t * M_PI))) * 0.5f;
		tangent = (1 - t) * a + t * b;
		return glm::normalize(tangent);
	}

	glm::vec3 GetTangent(float t, int i) const {
		int n = controlPoints.size() - 1;
		return glm::normalize(controlPoints[GetIndex(i - 1)] * ((-3 * t * t + 6 * t - 3) / 6) +
			controlPoints[GetIndex(i)] * ((9 * t * t - 12 * t) / 6) +
			controlPoints[GetIndex(i + 1)] * ((-9 * t * t + 6 * t + 3) / 6) +
			controlPoints[GetIndex(i + 2)] * ((3 * t * t) / 6));
	}

	std::vector<glm::vec3> CalculateRecursiveSubdivision(int i, float t0, float t1, glm::vec3 x0, glm::vec3 x1, glm::vec3 m0, glm::vec3 m1) {
		if (t1 - t0 < maximumSamplingDetail || x0 == x1) { // Avoid infinite recursion
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
		}
	}

	std::vector<glm::vec3> GetSplinePoints() {
		std::vector<glm::vec3> points;
		if (!isCyclic)
			points.push_back(controlPoints[0]);
		for (std::vector<glm::vec3> section : splineSections) {
			points.insert(points.end(), section.begin(), section.end());
		}
		if (!isCyclic)
			points.push_back(controlPoints[controlPoints.size() - 1]);
		return points;
	}

	// The control points that define the spline
	std::vector<glm::vec3> controlPoints;

	// The defined orientations for each control point
	std::vector<glm::vec3> orientations;

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

	// Whether to explicitly draw the points that are used to render the spline
	bool drawDebugPoints;

	// Whether this spline is cyclic or clamped.
	bool isCyclic = false;
};

#endif
