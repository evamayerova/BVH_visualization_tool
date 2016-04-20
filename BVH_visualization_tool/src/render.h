#ifndef RENDER_H
#define RENDER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

#include "BVHDrawer.h"
#include "SceneDrawer.h"

#include "structures.h"
#include "SceneImporter.h"

using namespace std;

namespace RenderType {
    enum Type {
        Tree,
        SceneView
    };
}

struct Camera {
	QVector3D pos, dir, upVector;
};

class Render
{
public:
    Render(RenderType::Type);
    Render(RenderType::Type, const string &sceneName);
    ~Render();

	virtual void draw() {}

	//void loadScene(const string &sceneName);
	void moveView(const QVector3D &change);
	void resetView();
	void scaleView(const float &scaleFactor);
    int initShaders(QOpenGLShaderProgram *shader_program, const char * vs, const char * fs);

    RenderType::Type renderType;
    
    QMatrix4x4 projection, view, model;
	QOpenGLShaderProgram shader;

	SceneImporter *sceneImporter;
    Scene *sc;
    vector<BVH*> bvhs;
	unsigned currentBVHIndex;
    float scaleFactor;
	QVector3D startingPosition, currentPosition;

private:
	void exportColors(const string &fileName);
};

#endif // RENDER_H
