#pragma once

#include "Mesh.h"
#include "bvh.h"
#include <QVector3D>

class BVHDrawer : protected QOpenGLFunctions_4_3_Core
{
public:
	BVHDrawer(BVH *b, QOpenGLShaderProgram *sp);
	~BVHDrawer();
	void generateMeshes();
	void changeScalarSet(int index);
	void highlightNode(const unsigned &index);
	void highlightNodes(const vector<unsigned> &indices);
	void showDisplayedNodes();
	void draw() const;
	void clearPath();

private:
	BVH *bvh;
	std::vector<Mesh*> meshes;
	QOpenGLShaderProgram *shaderProgram;
	vector<unsigned> highlightedNodes;
	int currentScalarSet;
	int currentNode;
};

