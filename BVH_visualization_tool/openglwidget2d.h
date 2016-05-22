#ifndef OPENGLWIDGET2D_H
#define OPENGLWIDGET2D_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QBasicTimer>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QWheelEvent>
#include <QDrag>
#include "src\TreeRender.h"
#include "mainwindow.h"

class OpenGlWidget2D : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    OpenGlWidget2D(QWidget *parent = NULL);
	void initializeRender(const string &sceneName);
	void initializeRender(Render *render);
	BVH * addBVH(const string &fileName);

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    virtual void timerEvent(QTimerEvent *);
    virtual void wheelEvent(QWheelEvent *e); 
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	QVector3D getWorldCoordinates(const QPoint &p);
	void setBlendType(BlendMode m);

	TreeRender *render;
	MainWindow *mw;
	string sceneName;

public slots:
    virtual void generateTree();

private:
    void scaleModel(int numSteps);

    float scaleFactor;
    int winWidth, winHeight;
    QBasicTimer timer;
	QVector3D dragStartPositionWorld;
	bool leftMousePressed;
	BlendMode blendType;
	//QOpenGLContext *context;
};

#endif // OPENGLWIDGET2D_H
