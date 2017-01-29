#ifndef CAMERA_H
#define CAMERA_H

class Camera
{

public:

	Camera() {}
	Camera(const glm::vec3& pos_, const glm::vec3& focusPos_, float fov_, float aspect_, float zNear_, float zFar_)
		: pos(pos_)
		, focusPos(focusPos_)
		, fov(fov_)
		, aspect(aspect_)
		, zNear(zNear_)
		, zFar(zFar_)
	{
	}

	virtual ~Camera() {}

	void Init(const glm::vec3& pos, const glm::vec3& focusPos, float fov, float aspect, float zNear, float zFar)
	{
		this->pos = pos;
		this->focusPos = focusPos;
		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;
	}
		
	void SetFieldOfView(float fov) { this->fov = fov; }
	void SetAspect(float aspect) { this->aspect = aspect; }
	void SetZNear(float zNear) { this->zNear = zNear; }
	void SetZFar(float zFar) { this->zFar = zFar; }
	
	void Update()
	{
		static const float speed = 0.0005f;
		if (Input::isKeyPressed(GLFW_KEY_W))
		{
			pos.y -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_S))
		{
			pos.y += speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_A))
		{
			pos.x += speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_D))
		{
			pos.x -= speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_Q))
		{
			pos.z += speed;
		}

		if (Input::isKeyPressed(GLFW_KEY_E))
		{
			pos.z -= speed;
		}
	}

	glm::mat4 ViewProjectionMatrix() const
	{
		glm::mat4 view = glm::lookAt(pos, focusPos, up);
		glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
		return projection * view;
	}

protected:

	float fov = 45.0f; // in degrees
	float aspect = 1.33f;
	float zNear = 0.1f;
	float zFar = 1000.0f;

	glm::vec3 pos;
	glm::vec3 focusPos;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

};

#endif