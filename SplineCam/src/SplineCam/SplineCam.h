#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"
#include "../Shaders/Shader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Camera/FollowSplineCamera.h"
#include "Camera/FPSCamera.h"
#include "Camera/FreeCamera.h"
#include "Spline/Spline.h"
#include "States/FreeCamState.h"
#include "States/SplineEditorState.h"
#include "States/FollowSplineState.h"

#include <cassert>
#include <memory>

class SplineCam : public InputListener
{
	enum class Mode
	{
		NONE,
		FREE_CAM,
		SPLINE_EDITOR,
		FOLLOW_SPLINE
	};

public:
	SplineCam() 
	{
		Init();
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

			case GLFW_KEY_1:
				SetMode(Mode::FREE_CAM);
				break;

			case GLFW_KEY_2:
				SetMode(Mode::SPLINE_EDITOR);
				break;

			case GLFW_KEY_3:
				SetMode(Mode::FOLLOW_SPLINE);
				break;
			default:
				if (state)
				{
					state->OnKeyPressed(key);
				}
				
				break;
		}
	};

	void OnKeyReleased(int key) override { };
	void OnMouseButtonPressed(int button, double x, double y) override { };
	void OnMouseButtonReleased(int button, double x, double y) override { };
	void OnMouseScroll(double xoffset, double yoffset) override { };
	
	void OnMouseMove(double x, double y) 
	{
		if (state)
		{
			state->OnMouseMove(x, y);
		}
	}

	void Update() 
	{
		if (state)
		{
			state->Update();
		}
	}

	void Render() 
	{
		if (state)
		{
			state->Render(shader);
		}

		DrawCubes();
	}
	
protected:

	void Init()
	{
		// init the vertex buffer object
		InitVBO();

		// init the indeces buffer object
		InitIBO();

		// init the vertex array object
		InitVAO();

		// load shader
		shader.Load("assets/Shaders/basic.vert", "assets/Shaders/basic.frag");

		// init cubes
		InitCubes();

		SetMode(Mode::FREE_CAM);
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

	void InitCubes()
	{
		// floor
		cubes[0].pos = glm::vec3(0.0f, 0.0f, 10.0f);
		cubes[0].scale = glm::vec3(20.0f, 0.0001f, 20.f);
		cubes[0].enabled = true;

		// others
		cubes[1].pos = glm::vec3(0.0f, 1.0f, 10.0f);
		cubes[1].color = glm::vec4(0.75f, 0.0f, 0.0f, 1.0f);
		cubes[1].enabled = true;

		cubes[2].pos = glm::vec3(5.0f, 3.5f, 5.0f);
		cubes[2].scale = glm::vec3(1.0f, 3.5f, 1.0f);
		cubes[2].color = glm::vec4(0.0f, 0.75f, 0.0f, 1.0f);
		cubes[2].enabled = true;

		cubes[3].pos = glm::vec3(-10.0f, 3.5f, 20.0f);
		cubes[3].scale = glm::vec3(1.0f, 3.5f, 1.0f);
		cubes[3].color = glm::vec4(0.0f, 0.0f, 0.75f, 1.0f);
		cubes[3].enabled = true;
	}

	void SetMode(Mode newMode)
	{
		if (newMode == mode)
		{
			// early exit if we already are in the same state
			return;
		}

		mode = newMode;

		// stop current state
		if (state)
		{
			state->Stop();
		}
		
		// set new state
		switch (mode)
		{
		case Mode::NONE:
			state.reset();
			break;
		case Mode::FREE_CAM:
			state.reset(new FreeCamState());
			break;
		case Mode::SPLINE_EDITOR:
			state.reset(new SplineEditorState());
			break;
		case Mode::FOLLOW_SPLINE:
			state.reset(new FollowSplineState());
			break;
		default:
			assert(false); // No state state
		}

		// start new state
		if (state)
		{
			state->Start();
		}
	}

	void DrawCubes()
	{
		const Camera* camera = state ? state->GetCamera() : nullptr;
		if (!camera)
		{
			assert(mode == Mode::NONE); // the state should have a camera!
			return;
		}

		const glm::mat4& viewProjection = camera->ViewProjectionMatrix();

		// use the shader
		shader.Use();	

		// tell the vertexArrayObject to be used
		glBindVertexArray(vertexArrayObject);

		glm::mat4 model;
		for (int i = 0; i < NUM_CUBES; i++)
		{
			Cube cube = cubes[i];
			if (cube.enabled)
			{
				model = glm::mat4();
				model = glm::translate(model, cube.pos) * glm::scale(model, cube.scale);
				shader.SetUniform("modelViewProjection", viewProjection * model);
				shader.SetUniform("color", cube.color);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)indices); // tell to draw cube by using the IBO
			}
		}

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

	// cubes
	struct Cube
	{
		bool enabled = false;

		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	static const int NUM_CUBES = 1024;
	Cube cubes[NUM_CUBES];

	Mode mode = Mode::NONE;
	std::unique_ptr<SplineCamState> state;

	bool wireframeMode = false;
};

#endif