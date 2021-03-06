#pragma once

#include "Scene.h"
#include "bvh.h"
#include "Mesh.h"
#include "../defines.h"

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

class SceneDrawer : protected QOpenGLFunctions_4_3_Core
{
public:
	SceneDrawer(Scene *sc, QOpenGLShaderProgram *
		, QOpenGLShaderProgram *bboxShader);
	~SceneDrawer();
	void draw(QMatrix4x4 *projection, QMatrix4x4 *view, QMatrix4x4 *model, PointLight *light);
	void drawToFile(const string &outputFile);
	void setSceneBuffers();
	void setBBoxBuffers();
	void setBBoxVertices(BVH *bvh, BVHNode *node);
	void unsetBBoxMesh();
	std::vector<unsigned> getPrimitiveIndices(BVH *bvh, BVHNode *n) const;
	std::vector<pair<unsigned, float>> getPrimitiveIndicesWithColors(BVH *bvh, BVHNode *n) const;

	bool showBBox;
	int cutDepth;
	int currentBVHIndex;
	int currentScalarSet;

private:
	Scene *scene;
	Mesh *sceneMesh, *bboxMesh;
	QMatrix4x4 *projection, *view, *model;
	QOpenGLShaderProgram *sceneShader, *bboxShader;
	std::vector<Color> sceneColors;
};

