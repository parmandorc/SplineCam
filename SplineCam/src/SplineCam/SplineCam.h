#ifndef SPLINE_CAM_H
#define SPLINE_CAM_H

#include "../Input/Input.h"

class SplineCam : public InputListener
{
public:
	SplineCam() {}
	~SplineCam() {}

	void OnKeyPressed(int key) override{ printf("Key  %d pressed\n", key); };
	void OnKeyReleased(int key) override { printf("Key  %d released\n", key); };
	void OnMouseButtonPressed(int button) override { printf("Mouse Button %d pressed\n", button); };
	void OnMouseButtonReleased(int button) override { printf("Mouse Button %d released\n", button); };
	void OnMouseScroll(double xoffset, double yoffset) override { printf("Scroll x = %f y = %f\n", xoffset, yoffset); };

	void Update() {}
	void Render() {}

private:

};

#endif