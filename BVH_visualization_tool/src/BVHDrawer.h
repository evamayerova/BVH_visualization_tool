#pragma once

#include "Mesh.h"
#include "bvh.h"
#include <QVector3D>
#include <assert.h>
#include <cstdio>
#include "lodepng/lodepng.h"

enum BlendMode {
	maxVal,
	minVal,
	aveVal,
	topVal
};

class BVHDrawer : protected QOpenGLFunctions_4_3_Core
{
public:
	BVHDrawer(
		BVH *b, 
		QOpenGLShaderProgram *sp, 
		QOpenGLShaderProgram *val, 
		QOpenGLShaderProgram *counts, 
		QOpenGLShaderProgram *transferBar,
		int treeDepth = MAX_TREE_DEPTH);
	~BVHDrawer();
	void generateMeshes();
	void changeScalarSet(int index, float polyExponent);
	void highlightNode(const unsigned &index);
	void highlightNodes(const vector<unsigned> &indices);
	void showDisplayedNodes();
	void draw();
	void drawToFile(const char *outputFile);
	void clearPath(float polyExponent);
	void setBlendMode(int current);
	void setShaderProgram(QOpenGLShaderProgram *sp);
	void reshape(const QSize &s);

	std::vector<Mesh*> meshes;
	Mesh *transferBarMesh;
	QSize screenSize;

private:
	void initScalarTexture(int i, GLuint &fbo, GLuint &tex);
	void initRenderBuffer(GLuint &fbo, GLuint &rbo);
	void reshapeScalarTexture(GLuint &tex);
	void scalarsToBuffer(int index, float polyExponent);
	float calculateValue(int scalarSetIndex, float val, float polynomDegree);

	BVH *bvh;
	vector<unsigned> highlightedNodes;
	int currentScalarSet;
	int currentNode;
	bool texturesInitialized;
	BlendMode blendMode;
	GLuint fboValues, fboCounts;
	GLuint textureValues, textureCounts;
	GLint screenFrameBuffer;
	GLuint fileFrameBuffer, fileRenderBuffer;
	QOpenGLShaderProgram *shaderProgram, *transferBarShader;
	QOpenGLShaderProgram *textureRenderCountsShader, *textureRenderValuesShader;
};

