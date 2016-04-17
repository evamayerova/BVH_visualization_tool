#pragma once
#include "render.h"

class TreeRender : 
	public Render
{
public:
	TreeRender();
	TreeRender(const string &sceneName);
	~TreeRender();

	void generateScalarSets();
	void removeScalarSets();
	void exportColors(const string &fileName);
	void loadScene(const string & sceneName);
	void draw();
	int pick(const QVector2D &point);
	void displayPath(const std::vector<unsigned> &indices);
	BVHDrawer *drawer;
	Camera cam;
};

