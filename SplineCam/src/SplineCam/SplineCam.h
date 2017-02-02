#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"
#include "../Shaders/Shader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "Camera/Camera.h"
#include "Spline/Spline.h"

class SplineCam : public InputListener
{
public:
	SplineCam() 
	{
		// init the vertex buffer object
		InitVBO();

		// init the indeces buffer object
		InitIBO();

		// init the vertex array object
		InitVAO();

		// load shader
		shader.Load("assets/Shaders/basic.vert", "assets/Shaders/basic.frag");

		// init camera
		camera.Init(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), 70.0f, 800.0f/600.0f, 0.1f, 1000.0f);

		// init spline
		spline.Init(std::vector<glm::vec3>({
			glm::vec3(-5.0f, -7.5f, 0.0f),
			glm::vec3(-7.5f, 0.0f, 0.0f),
			glm::vec3(0.0f, 3.0f, 0.0f),
			glm::vec3(7.5f, 0.0f, 0.0f),
			glm::vec3(5.0f, -7.5f, 0.0f),
		}));
	}

	~SplineCam() 
	{
		Terminate();
	}

	void OnKeyPressed(int key) override 
	{
		switch(key)
		{
			case GLFW_KEY_F1:
				ToggleWireframeMode();
				break;

			case GLFW_KEY_TAB:
				if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
					spline.PreviousControlPoint();
				else
					spline.NextControlPoint();
				break;

			case GLFW_KEY_SPACE:
				if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
					spline.DeleteControlPoint();
				else
					spline.CreateControlPoint();
				break;
		}
	};

	void OnKeyReleased(int key) override { };
	void OnMouseButtonPressed(int button, double x, double y) override { printf("Mouse button %d pressed at ( %f , %f )\n", button, x, y); };
	void OnMouseButtonReleased(int button, double x, double y) override { printf("Mouse button %d released at ( %f , %f )\n", button, x, y); };
	void OnMouseScroll(double xoffset, double yoffset) override { };

	void Update() 
	{
		UpdateCube();
		camera.Update();
		UpdateSpline();

		animationFrame = fmodf(animationFrame + 0.00025f, 1);
	}

	void Render() 
	{
		DrawCube();
		spline.Render(camera.ViewProjectionMatrix(), shader);
		DrawAnimatedPoint();
	}
	
protected:

	void UpdateCube()
	{
		static const float speed = 0.0005f;
		if (Input::isKeyPressed(GLFW_KEY_UP))
		{
			cubePos.y += speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_DOWN))
		{
			cubePos.y -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_LEFT))
		{
			cubePos.x -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_RIGHT))
		{
			cubePos.x += speed;
		}

		cubeRotY += 0.0005f;
	}

	void UpdateSpline() {
		static const float speed = 0.01f;
		if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			int x = Input::isKeyPressed(GLFW_KEY_D) - Input::isKeyPressed(GLFW_KEY_A);
			int y = Input::isKeyPressed(GLFW_KEY_W) - Input::isKeyPressed(GLFW_KEY_S);
			int z = Input::isKeyPressed(GLFW_KEY_Q) - Input::isKeyPressed(GLFW_KEY_E);
			if (x != 0 || y != 0 || z != 0) {
				spline.TranslateControlPoint(glm::vec3(x, y, z) * speed);
			}
		}
	}

	void InitVBO()
	{
		// create one buffer in the GPU, use it as an array buffer and set the data
		glGenBuffers(1, &vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	void InitIBO()
	{
		// create one buffer in the GPU, use it as an element array buffer and set the data
		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	void InitVAO()
	{
		// create one vertex array object for drawing and use it. This vertexArrayObject is the one who will deal with the vertex buffer
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		// Tell the vertex shader how to interpret the buffer data. This information is needed for the active vertexArrayObject
		// The 0 attribute(pos) has 3 elements (x,y,z) of type GL_FLOAT
		// The stride to the next 0 attribute is zero bytes because there are no other attributes in between
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		// Enable the 0 attribute
		glEnableVertexAttribArray(0);
	}

	void DrawCube()
	{
		// build modelViewProjection matrix
		glm::mat4 model;
		model = glm::translate(model, cubePos) * glm::rotate(model, cubeRotY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(model, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelViewProjection = camera.ViewProjectionMatrix() * model;

		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", modelViewProjection);

		// tell the vertexArrayObject to be used
		glBindVertexArray(vertexArrayObject);

		// tell to draw triangles by using the IBO
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)indices);

		// do not use the vertexArrayObject anymore
		glBindVertexArray(0);
	}

	void DrawAnimatedPoint() {
		// build modelViewProjection matrix
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f)) * glm::rotate(model, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 modelViewProjection = camera.ViewProjectionMatrix() * model;

		// use the shader
		shader.Use();

		// set uniforms
		shader.SetUniform("modelViewProjection", modelViewProjection);

		// draw the control points
		glm::vec3 point = spline.GetPoint(animationFrame);
		glm::vec3 tangent = spline.GetTangent(animationFrame);
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3f(point.x, point.y, point.z);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(point.x, point.y, point.z);
		glVertex3f(point.x + tangent.x, point.y + tangent.y, point.z + tangent.z);
		glEnd();
	}

	void Terminate()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(1, &vertexBufferObject);
		glDeleteBuffers(1, &indexBufferObject);
	}

	void ToggleWireframeMode()
	{
		wireframeMode = !wireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);		
	}

private:

	// vertices of the cube
	GLfloat vertices[24] =
	{
		-1.0f,	1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,	1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f
	};


	// indices to cube vertices
	GLuint indices[36] =
	{
	   // tri 1      tri 2
		0, 2, 1,	0, 3, 2,	// front face
		4, 6, 5,    4, 7, 6,	// back face	
		4, 3, 0,    4, 7, 3,	// left face	
		1, 6, 5,    1, 2, 6,	// right face	
		4, 1, 5,    4, 0, 1,	// top face	
		7, 2, 6,    7, 3, 2		// bottom face	
	};
	
	// vbo, vao, ibo 
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
	GLuint vertexArrayObject; 
	
	// shader
	Shader shader;

	// cubePos
	glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, -10.0f);
	float cubeRotY = 0.0f;

	// animatedPoint
	float animationFrame = 0.0f;

	// camera
	Camera camera;

	// spline
	Spline spline;

	bool wireframeMode = false;
};

#endif