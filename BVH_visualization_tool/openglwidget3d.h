#ifndef OpenGlWidget3D_H
#define OpenGlWidget3D_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <QWidget>
#include <QOpenGLWidget>
#include <QBasicTimer>
#include <QOpenGLDebugLogger>
#include <QMouseEvent>
#include "src\SceneRender.h"
#include "mainwindow.h"
#include "src\TrackballCamera.h"

//using namespace glm;

class OpenGlWidget3D : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    OpenGlWidget3D(QWidget *parent = NULL);

	QVector3D trackBallMapping(const QPoint &p);
	void initializeRender(const string &sceneName, const string &camFile, const string &lightsFile);
	void setFarPlane(const float &f);
	void setNearPlane(const float &n);
	float getFarPlane();
	float getNearPlane();
	void addBVH(BVH *b);

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
	virtual void wheelEvent(QWheelEvent *e);
    virtual void timerEvent(QTimerEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);

	SceneRender *render;
	MainWindow *mw;
	string sceneName;

private:
    int winWidth, winHeight;
	float nearPlane, farPlane; 
	bool mousePressed;
	bool rotateMovement;
    QBasicTimer timer;
	QPoint dragStartPosition;
	QPoint lastRotPoint, currenRotPoint;
	TrackballCamera t_camera;
};

#endif // OpenGlWidget3D_H
