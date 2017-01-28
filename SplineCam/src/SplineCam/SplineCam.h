#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"

static const GLchar* s_vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"void main()"
"{"
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);"
"}";

static const GLchar* s_fragmentShaderSrc =
"#version 330 core\n"
"out vec4 frag_color;"
"void main()"
"{"
"	frag_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);"
"}";

class SplineCam : public InputListener
{
public:
	SplineCam() 
	{
		glGenBuffers(1, &vertexBufferObject); // generate one buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); // use vertexBufferObject as an array buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		// create shaders
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
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

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &s_fragmentShaderSrc, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLchar shaderInfoLog[512];
			glGetShaderInfoLog(fragmentShader, sizeof(shaderInfoLog), nullptr, shaderInfoLog);
			printf("Fragment shader failed to compile.\n %s", shaderInfoLog);
		}

		// create program
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
		
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	~SplineCam() 
	{
		glDeleteProgram(shaderProgram);
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(1, &vertexBufferObject);
	}

	void OnKeyPressed(int key) override { };
	void OnKeyReleased(int key) override { };
	void OnMouseButtonPressed(int button) override { };
	void OnMouseButtonReleased(int button) override { };
	void OnMouseScroll(double xoffset, double yoffset) override { };

	void Update() 
	{
	}

	void Render() 
	{
		glUseProgram(shaderProgram);

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
	}

private:

	GLfloat vertices[9] = 
	{
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	   -0.5f, -0.5f, 0.0f 
	};

	GLuint vertexBufferObject;
	GLuint vertexArrayObject; 

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint shaderProgram;
};

#endif