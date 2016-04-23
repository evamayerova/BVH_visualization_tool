#pragma once
#include "render.h"

class SceneRender :
	public Render
{
public:
	SceneRender();
	SceneRender(const string &sceneName, const string &camFile = "", const string &lightsFile = "");
	~SceneRender();

	const bool castRay(BVHNode *node, unsigned nodeIndex, ray & r);
	const bool rayTriangle(ray & r, Triangle * t);
	
	void loadCameras(const string &camFilePath);
	void loadLights(const string &lightsFilePath);
	void switchCamera(int camIndex);
	void addBBox(BVHNode *n);
	void removeBBox();
	void loadScene(const string & sceneName);
	void draw();
	int pick(ray &r);
	void scrollView(int numSteps);

	int currentCamera;
	vector<Camera*> cams;
	Camera currCam;
	QOpenGLShaderProgram bboxShader;
	PointLight light;
	SceneDrawer *drawer;
};

