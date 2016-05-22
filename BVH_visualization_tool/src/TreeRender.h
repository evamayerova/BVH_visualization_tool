#pragma once
#include "render.h"
#include "../defines.h"
#include <QOpenGLFramebufferObject>

#include <QElapsedTimer>

class TreeRender : 
	public Render
{
public:
	TreeRender();
	TreeRender(Render *render);
	TreeRender(const string &sceneName);
	~TreeRender();

	void removeScalarSets();
	void exportColors(const string &fileName);
	void loadScene(const string & sceneName);
	bool addBVH(const string &fileName);
	void changeTreeDepth(int newDepth, int scalarSet);
	void draw();
	void moveView(const QVector3D &change);	
	int pick(const QVector2D &point);
	void displayPath(const std::vector<unsigned> &indices);
	void changeCurrentShader(int current);
	void changeCurrentBVH(int current);
	void changeBlendMode(int mode);
	void setWiewportSize(const QSize &s);
	void setDefaultFrameBuffer(GLint fb);
	void screenshot(const char *outputFile);

	QOpenGLShaderProgram solidBoxShader, lineBoxShader, solidRingShader, lineRingShader;
	QOpenGLShaderProgram textureRenderCountsShader, textureRenderValuesShader;
	QOpenGLShaderProgram transferBarShader;
	QOpenGLShaderProgram *currentShader;
	vector<BVHDrawer*> drawers;
	Camera cam;
	float polynomDegree;

private:
	GLint defaultFrameBuffer;
	int drawMode;
	QSize *size;
};

