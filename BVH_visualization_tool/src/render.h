#ifndef RENDER_H
#define RENDER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QElapsedTimer>

#include "BVHDrawer.h"
#include "SceneDrawer.h"

#include "structures.h"
#include "SceneImporter.h"

#include "../defines.h"

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
	Render(RenderType::Type type, const Render &render),
    Render(RenderType::Type, const string &sceneName);
    ~Render();

	virtual void draw() {}
	virtual void moveView(const QVector3D &) {}
	virtual void screenshot(const string &) {}

	//void loadScene(const string &sceneName);
	void resetView();
	void scaleView(const float &scaleFactor);
    int initShaders(QOpenGLShaderProgram *shader_program, const char * vs, const char * fs, const char *gs = "");

    RenderType::Type renderType;
    
    QMatrix4x4 projection, view, model;
	QOpenGLShaderProgram shader;

	SceneImporter *sceneImporter;
    Scene *sc;
    vector<BVH*> bvhs;
	unsigned currentBVHIndex;
    float scaleFactor;
	QVector3D startingPosition, currentPosition;
	QVector2D hPixel;

private:
	void exportColors(const string &fileName);
};

#endif // RENDER_H
