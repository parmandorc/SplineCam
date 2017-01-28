#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"

// basic vertex shader into an string
static const GLchar* s_vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"void main()"
"{"
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);"
"}";

// basic fragment shader into an string
static const GLchar* s_fragmentShaderSrc =
"#version 330 core\n"
"out vec4 frag_color;"
"void main()"
"{"
"	frag_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
"}";

// SplineCam
//
class SplineCam : public InputListener
{
public:
	SplineCam() 
	{
		// init the vertex buffer object
		InitVBO();

		// init the vertex array object
		InitVAO();

		// init the shader
		InitShader();
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
		if (Input::isKeyPressed(GLFW_KEY_W))
		{
			printf("Key W pressed\n");
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
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

	void InitShader()
	{
		// create vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &s_vertexShaderSrc, nullptr);
		glCompileShader(vertexShader);

		GLint result;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLchar shaderInfoLog[512];
			glGetShaderInfoLog(vertexShader, sizeof(shaderInfoLog), nullptr, shaderInfoLog);
			printf("Vertex shader failed to compile.\n %s", shaderInfoLog);
		}

		// create fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &s_fragmentShaderSrc, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLchar shaderInfoLog[512];
			glGetShaderInfoLog(fragmentShader, sizeof(shaderInfoLog), nullptr, shaderInfoLog);
			printf("Fragment shader failed to compile.\n %s", shaderInfoLog);
		}

		// create the shader program and attach the shaders to use
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
		if (!result)
		{
			GLchar programInfoLog[512];
			glGetProgramInfoLog(shaderProgram, sizeof(programInfoLog), nullptr, programInfoLog);
			printf("Shader program failed to link.\n %s", programInfoLog);
		}

		// delete the created shaders as they are already linked to the program
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void DrawQuad()
	{
		// tell the program to be used
		glUseProgram(shaderProgram);

		// tell the vertexArrayObject to be used
		glBindVertexArray(vertexArrayObject);

		// tell to draw triangles starting from the 0 index(pos) of the active vertexArrayObject(that has 6 vertices).
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// do not use the vertexArrayObject anymore
		glBindVertexArray(0);
	}

	void Terminate()
	{
		// delete shader program and genereted vao and vbo
		glDeleteProgram(shaderProgram);
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(1, &vertexBufferObject);
	}

	void ToggleWireframeMode()
	{
		wireframeMode = !wireframeMode;
		glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);		
	}

private:

	// vertices of the quad (2 triangles
	GLfloat vertices[18] = 
	{
		// triangle 1
		-0.5f,	0.5f, 0.0f,
		 0.5f,	0.5f, 0.0f,
	     0.5f, -0.5f, 0.0f,

	   // triangle 2
		-0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	// vbo, vao and shader program
	GLuint vertexBufferObject;
	GLuint vertexArrayObject; 
	GLuint shaderProgram;

	bool wireframeMode = false;
};

#endif