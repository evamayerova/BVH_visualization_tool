#include "openglwidget2d.h"
#include <cmath>
#include <assert.h>

#ifdef RESPONSE_TIMES
#include <time.h>
#endif

OpenGlWidget2D::OpenGlWidget2D(QWidget *parent) : QOpenGLWidget(parent)
{
	/*
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	ctx = new QOpenGLContext();
	QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);
	*/
}

BVH * OpenGlWidget2D::addBVH(const string &fileName)
{
	this->makeCurrent();
	assert(glGetError() == GL_NO_ERROR);
	if (render->addBVH(fileName))
		return render->bvhs[render->bvhs.size() - 1];

	//QOpenGLContext *context = QOpenGLContext::currentContext();
	resizeGL(width(), height());
	assert(glGetError() == GL_NO_ERROR);
	return NULL;
}

void OpenGlWidget2D::initializeRender(const string &sceneName)
{
	this->makeCurrent();
	delete render;
	render = new TreeRender(sceneName);
	render->hPixel = QVector2D(1.f / width(), 1.f / height());
	if (!render)
		throw "No render set";
	
	GLint fb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
	render->setDefaultFrameBuffer(fb);

	mw->AddRender(render);
	resizeGL(width(), height());
}

void OpenGlWidget2D::initializeRender(Render *render)
{
	this->makeCurrent();
	delete this->render;
	this->render = new TreeRender(render, QSize(winWidth, winHeight));
	this->render->hPixel = QVector2D(1.f / width(), 1.f / height());
	if (!render)
		throw "No render set";

	GLint fb;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
	this->render->setDefaultFrameBuffer(fb);

	mw->AddRender(this->render);
	resizeGL(width(), height());
}

void OpenGlWidget2D::initializeGL()
{
	this->makeCurrent();
	initializeOpenGLFunctions();
	render = NULL;
	blendType = maxVal;
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glEnable(GL_TEXTURE_2D);
	timer.start(12, this);
	resizeGL(width(), height());
}

void OpenGlWidget2D::paintGL()
{
	this->makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	assert(glGetError() == GL_NO_ERROR);
	if (render)
	{
		render->draw();
		resizeGL(winWidth, winHeight);
		assert(glGetError() == GL_NO_ERROR);
	}
	this->update();
}

void OpenGlWidget2D::resizeGL(int w, int h)
{
	this->makeCurrent();
	winWidth = w;
	winHeight = h;

	glViewport(0, 0, w, h);
	if (render) {
		render->setWiewportSize(size());
		render->projection.setToIdentity();
		render->hPixel = QVector2D(1.f / width(), 1.f / height());
	}
}

void OpenGlWidget2D::timerEvent(QTimerEvent *)
{
	update();
}


void OpenGlWidget2D::scaleModel(int numSteps)
{
	scaleFactor = numSteps * 0.1;
	render->scaleView(scaleFactor);// += scaleFactor;
}

void OpenGlWidget2D::wheelEvent(QWheelEvent *event)
{
	if (!render)
		return;

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Horizontal) {
		//scrollHorizontally(numSteps);
	}
	else {
		scaleModel(numSteps);
	}
	event->accept();
}

QVector3D OpenGlWidget2D::getWorldCoordinates(const QPoint &p)
{
	QVector3D dnc; // device normalized coordinates
	dnc[0] = p.x() * 2 / float(winWidth) - 1;
	dnc[1] = 1 - p.y() * 2 / float(winHeight);
	dnc[2] = 1.f;

	QVector4D posClip(dnc[0], dnc[1], -1.f, 1.f);
	QVector4D posEye = posClip;
	posEye = QVector4D(posEye[0], posEye[1], -1.f, 0.f);
	QVector4D posWorld = posEye;
	QVector4D posModel = render->model.inverted() * posWorld;
	return QVector3D(posModel).normalized();
}

void OpenGlWidget2D::setBlendType(BlendMode m)
{
	render->changeBlendMode(m);
}

void OpenGlWidget2D::mousePressEvent(QMouseEvent *event)
{
	if (!render)
		return;

	switch (event->button()) {
	case Qt::LeftButton: {
		QPoint p = event->pos();

#ifdef RESPONSE_TIMES
		srand(time(NULL));
		QElapsedTimer t;
		qint64 totalTime = 0;
		int nrOfIterations = 50;
		int i = 0;

		while (i < nrOfIterations)
		{
			p.setX(rand() % winWidth);
			p.setY(rand() % winHeight);
			t.start();
#endif
		// picking
		QVector2D dnc; // device normalized coordinates
		dnc[0] = p.x() * 2 / float(winWidth) - 1;
		dnc[1] = 1 - p.y() * 2 / float(winHeight);

		QVector4D transformedPos = render->model.inverted() * QVector4D(dnc.x(), dnc.y(), 0.0, 1.0);
		int pickedNode = render->pick(QVector2D(transformedPos.x(), transformedPos.y()));
		mw->PrintNodeInfo(pickedNode);

#ifdef RESPONSE_TIMES
		if (pickedNode == -1)
			continue;

		qint64 elapsed = t.elapsed();
		totalTime += elapsed;
		i++;
		}

		std::ofstream measures;
		measures.open(measureFileName, std::ios::app);
		measures << totalTime / nrOfIterations << " \\\\ " << endl;
		measures.close();
#endif
		break;
	}
	case Qt::RightButton: {
		// start moving
		leftMousePressed = true;

		dragStartPositionWorld = getWorldCoordinates(event->pos());

		break;
	}
	}
}

void OpenGlWidget2D::mouseMoveEvent(QMouseEvent *event)
{
	if (!render)
		return;

	if (!(event->buttons() & Qt::RightButton))
		return;

	QVector3D eventWorld = getWorldCoordinates(event->pos());
	QVector3D change = eventWorld - dragStartPositionWorld;
	render->moveView(change);
	dragStartPositionWorld = eventWorld;
}

void OpenGlWidget2D::mouseReleaseEvent(QMouseEvent *)
{
	if (!render)
		return;

	leftMousePressed = false;
}

void OpenGlWidget2D::generateTree()
{
	render->bvhs[render->currentBVHIndex]->generateTree();
}
