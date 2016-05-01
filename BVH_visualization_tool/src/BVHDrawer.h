#pragma once

#include "Mesh.h"
#include "bvh.h"
#include <QVector3D>
#include <assert.h>

class BVHDrawer : protected QOpenGLFunctions_4_3_Core
{
public:
	BVHDrawer(BVH *b, QOpenGLShaderProgram *sp, int treeDepth = MAX_TREE_DEPTH);
	~BVHDrawer();
	void generateMeshes();
	void changeScalarSet(int index);
	void highlightNode(const unsigned &index);
	void highlightNodes(const vector<unsigned> &indices);
	void showDisplayedNodes();
	void draw() const;
	void clearPath();
	void setShaderProgram(QOpenGLShaderProgram *sp);

	std::vector<Mesh*> meshes;
private:
	BVH *bvh;
	QOpenGLShaderProgram *shaderProgram;
	vector<unsigned> highlightedNodes;
	int currentScalarSet;
	int currentNode;
};

