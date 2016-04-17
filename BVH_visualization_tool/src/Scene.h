#pragma once

#include <vector>
#include <string>
#include <QVector3D>

#include "colorMapping.h"

struct Triangle {
	QVector3D vertices[3];
	Color color;
};

class Scene
{
public:
	Scene() {}
	~Scene();

	std::vector<Triangle> mTriangles;
	std::vector<unsigned int> mTriangleIdx;
};
