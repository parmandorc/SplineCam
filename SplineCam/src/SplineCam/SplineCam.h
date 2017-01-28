#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"
#include "../Shaders/Shader.h"

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
	}

	~SplineCam() 
	{
		Terminate();
	}

	void OnKeyPressed(int key) override 
	{
		if (key == GLFW_KEY_F1)
		{
			ToggleWireframeMode();
		}
	};

	void OnKeyReleased(int key) override { };
	void OnMouseButtonPressed(int button, double x, double y) override { printf("Mouse button %d pressed at ( %f , %f )\n", button, x, y); };
	void OnMouseButtonReleased(int button, double x, double y) override { printf("Mouse button %d released at ( %f , %f )\n", button, x, y); };
	void OnMouseScroll(double xoffset, double yoffset) override { };

	void Update() 
	{
		static const float speed = 0.0001f;
		if (Input::isKeyPressed(GLFW_KEY_W))
		{
			quadPosOffset.y += speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_S))
		{
			quadPosOffset.y -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_A))
		{
			quadPosOffset.x -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_D))
		{
			quadPosOffset.x += speed;
		}
	}

	void Render() 
	{
		DrawQuad();
	}

	
protected:

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

	void DrawQuad()
	{
		// use the shader
		shader.Use();

		// set posOffset uniform
		shader.SetUniform("posOffset", quadPosOffset);

		// tell the vertexArrayObject to be used
		glBindVertexArray(vertexArrayObject);

		// tell to draw triangles starting from the 0 index(pos) of the active vertexArrayObject(that has 6 vertices).
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)indices);

		// do not use the vertexArrayObject anymore
		glBindVertexArray(0);
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

	// vertices of the quad
	GLfloat vertices[12] = 
	{
		-0.5f,	0.5f, 0.0f,
		 0.5f,	0.5f, 0.0f,
	     0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	// indices to the vertex of the quad
	GLuint indices[6] =
	{
		0, 1, 2, // first triangle
		0, 2, 3  // second triangle
	};

	// vbo, vao, ibo 
	GLuint vertexBufferObject;
	GLuint indexBufferObject;
	GLuint vertexArrayObject; 
	
	// shader
	Shader shader;

	glm::vec3 quadPosOffset;

	bool wireframeMode = false;
};

#endif