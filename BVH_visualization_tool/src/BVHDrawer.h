#pragma once

#include "Mesh.h"
#include "bvh.h"
#include <QVector3D>
#include <assert.h>


enum BlendMode {
	maxVal,
	minVal,
	aveVal,
	topVal
};

class BVHDrawer : protected QOpenGLFunctions_4_3_Core
{
public:
	BVHDrawer(BVH *b, QOpenGLShaderProgram *sp, QOpenGLShaderProgram *val, QOpenGLShaderProgram *counts, int treeDepth = MAX_TREE_DEPTH);
	~BVHDrawer();
	void generateMeshes();
	void changeScalarSet(int index);
	void highlightNode(const unsigned &index);
	void highlightNodes(const vector<unsigned> &indices);
	void showDisplayedNodes();
	void draw();
	void clearPath();
	void setBlendMode(int current);
	void setShaderProgram(QOpenGLShaderProgram *sp);
	void reshape(const QSize &s);

	std::vector<Mesh*> meshes;
	QSize screenSize;

private:
	void initScalarTexture(int i, GLuint &fbo, GLuint &tex);
	void reshapeScalarTexture(GLuint &tex);

	BVH *bvh;
	vector<unsigned> highlightedNodes;
	int currentScalarSet;
	int currentNode;
	bool texturesInitialized;
	BlendMode blendMode;
	GLuint fboValues, fboCounts;
	GLuint textureValues, textureCounts;
	GLint screenFrameBuffer;
	QOpenGLShaderProgram *shaderProgram;
	QOpenGLShaderProgram *textureRenderCountsShader, *textureRenderValuesShader;
};

