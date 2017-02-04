#ifndef SPLINE_CAM_STATE_H
#define  SPLINE_CAM_STATE_H

class SplineCamState
{	
public:
	SplineCamState() {}
	virtual ~SplineCamState() {}

	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Update() = 0;
	virtual void Render(Shader& shader) = 0;

	virtual void OnKeyPressed(int key) {}
	virtual void OnKeyReleased(int key) {}
	virtual void OnMouseButtonPressed(int button, double x, double y) {}
	virtual void OnMouseButtonReleased(int button, double x, double y) {}
	virtual void OnMouseMove(double x, double y) {}

	virtual const Camera* GetCamera() const { return nullptr; }
};

#endif
