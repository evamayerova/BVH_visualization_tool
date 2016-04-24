#pragma once

#include <vector>
#include <string>
#include <QVector3D>
#include <QMatrix4x4>

#include "colorMapping.h"

struct PointLight {
	QVector3D position;
	QVector3D diffuse;
};

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
	std::vector<std::vector<unsigned int> > mTriangleIdx;
};
