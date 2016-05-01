#pragma once
#include "render.h"

class TreeRender : 
	public Render
{
public:
	TreeRender();
	TreeRender(const string &sceneName);
	~TreeRender();

	void removeScalarSets();
	void exportColors(const string &fileName);
	void loadScene(const string & sceneName);
	bool addBVH(const string &fileName);
	void changeTreeDepth(int newDepth, int scalarSet);
	void draw();
	int pick(const QVector2D &point);
	void displayPath(const std::vector<unsigned> &indices);
	void changeCurrentShader(int current);
	void changeCurrentBVH(int current);

	QOpenGLShaderProgram solidBoxShader, lineBoxShader, solidRingShader, lineRingShader;
	QOpenGLShaderProgram *currentShader;
	vector<BVHDrawer*> drawers;
	Camera cam;
};

