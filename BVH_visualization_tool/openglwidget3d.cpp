#include "OpenGlWidget3D.h"

#ifdef RESPONSE_TIMES
#include <time.h>
#endif 

OpenGlWidget3D::OpenGlWidget3D(QWidget *parent) : QOpenGLWidget(parent)
{
	QOpenGLContext *ctx = QOpenGLContext::currentContext();
	QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);

	rotateMovement = false;
}

QVector3D OpenGlWidget3D::trackBallMapping(const QPoint & p)
{
	QVector3D arcballOrigin = render->currCam.pos + 20 * render->currCam.dir;
	QVector3D v;
	float d;
	v[0] = 2.0 * p.x() / float(winWidth) - 1.0;
	v[1] = -(2.0 * p.y() / float(winHeight) - 1.0);
	v[2] = 0.0;

	float square = v.x() * v.x() + v.y() * v.y();
	//float square = (v.x() - arcballOrigin.x()) * (v.x() - arcballOrigin.x()) + 
	//			   (v.y() - arcballOrigin.y()) * (v.y() - arcballOrigin.y());

	if (square <= 1 * 1)
		v[2] = sqrt(1 * 1 - square);
	else
		v.normalize();
	return v;

}

void OpenGlWidget3D::initializeRender(const string &sceneName, const string &camFile, const string &lightsFile)
{
	this->makeCurrent();
	delete render;
	render = new SceneRender(sceneName, camFile, lightsFile);
	render->hPixel = QVector2D(1.f / width(), 1.f / height());
	if (!render)
		throw "No render set";
	farPlane = 5 *
		max(
			max(render->bvhs[0]->mNodes[0].bounds[1][0] - render->bvhs[0]->mNodes[0].bounds[0][0],
				render->bvhs[0]->mNodes[0].bounds[1][1] - render->bvhs[0]->mNodes[0].bounds[0][1]),
			render->bvhs[0]->mNodes[0].bounds[1][2] - render->bvhs[0]->mNodes[0].bounds[0][2]);

	resizeGL(width(), height());
	mw->AddRender(render);
}

void OpenGlWidget3D::initializeRender(Render * r, const string & camFile, const string & lightsFile)
{
	this->makeCurrent();
	delete render;
	render = new SceneRender(r, camFile, lightsFile);
	render->hPixel = QVector2D(1.f / width(), 1.f / height());
	if (!render)
		throw "No render set";
	farPlane = 5 *
		max(
			max(render->bvhs[0]->mNodes[0].bounds[1][0] - render->bvhs[0]->mNodes[0].bounds[0][0],
				render->bvhs[0]->mNodes[0].bounds[1][1] - render->bvhs[0]->mNodes[0].bounds[0][1]),
			render->bvhs[0]->mNodes[0].bounds[1][2] - render->bvhs[0]->mNodes[0].bounds[0][2]);

	resizeGL(width(), height());
	mw->AddRender(render);
}

void OpenGlWidget3D::initializeGL()
{
	this->makeCurrent();
	initializeOpenGLFunctions();
	render = NULL;
	glClearColor(0.2, 0.2, 0.2, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	timer.start(12, this);
	nearPlane = 0.1f;
	farPlane = 100.f;
}

void OpenGlWidget3D::paintGL()
{
	this->makeCurrent();

	if (lastRotPoint != currenRotPoint)
	{
		QVector3D va = trackBallMapping(lastRotPoint);
		QVector3D vb = trackBallMapping(currenRotPoint);
		float angle = acos(min(1.0f, QVector3D::dotProduct(va, vb)));
		QVector3D axisCamCoords = QVector3D::crossProduct(va, vb);

		QMatrix4x4 camera2object = (render->view * render->model).inverted();
		QVector3D axisObjectCoord = QVector3D(camera2object * QVector4D(axisCamCoords, 0.0));
		render->model.rotate(angle * 180 / M_PI, axisObjectCoord);

		lastRotPoint = currenRotPoint;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (render)
		render->draw();

	this->update();
}

void OpenGlWidget3D::setNearPlane(const float &n)
{
	nearPlane = n;
	resizeGL(width(), height());
}

float OpenGlWidget3D::getFarPlane() const
{
	return farPlane;
}

float OpenGlWidget3D::getNearPlane() const
{
	return nearPlane;
}

void OpenGlWidget3D::addBVH(BVH *b, const vector<unsigned> &indices)
{
	render->bvhs.push_back(b);
	render->sc->mTriangleIdx.push_back(indices);
}

void OpenGlWidget3D::setFarPlane(const float &f)
{
	farPlane = f;
	resizeGL(width(), height());
}

void OpenGlWidget3D::resizeGL(int w, int h)
{
	this->makeCurrent();
	winWidth = w;
	winHeight = h;

	glViewport(0, 0, w, h);

	if (render) {
		render->projection.setToIdentity();
		render->projection.perspective(60.0f, w / (float)h, nearPlane, farPlane);
	}
}

void OpenGlWidget3D::timerEvent(QTimerEvent *)
{
	update();
}

void OpenGlWidget3D::wheelEvent(QWheelEvent * event)
{
	if (!render)
		return;

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Horizontal) {
		//scrollHorizontally(numSteps);
	}
	else {
		float mult = farPlane / NUMBER_OF_SCROLL_STEPS;
		numSteps *= mult;
		render->scrollView(numSteps);
		//render->view.scale(numSteps >= 0 ? numSteps : 1/(-numSteps));
	}
	event->accept();
}

void OpenGlWidget3D::mousePressEvent(QMouseEvent *event)
{
	if (!render)
		return;

	this->makeCurrent();

	switch (event->button()) {
	case Qt::LeftButton:
	{
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
			QVector3D dnc; // device normalized coordinates
			dnc[0] = p.x() * 2 / float(winWidth) - 1;
			dnc[1] = 1 - p.y() * 2 / float(winHeight);
			dnc[2] = 1.f;

			QVector3D rayPosEye(0.f, 0.f, 0.f);
			QVector3D rayPosWorld = QVector3D(render->view.inverted() * QVector4D(rayPosEye, 1.0));

			QVector4D rayDirClip(dnc[0], dnc[1], -1.f, 1.f);
			QVector4D rayDirEye = render->projection.inverted() * rayDirClip;
			rayDirEye = QVector4D(rayDirEye[0], rayDirEye[1], -1.f, 0.f);
			QVector3D rayDirWorld = QVector3D(render->view.inverted() * rayDirEye).normalized();

			QVector3D rayPosModel = QVector3D(render->model.inverted() * rayPosWorld);
			QVector3D rayDirModel = QVector3D(render->model.inverted() * rayDirWorld);

			ray *r = new ray(rayPosModel, rayDirModel);
			render->pick(*r);
			mw->DisplayBVHPath(r->BVHListNode);

#ifdef RESPONSE_TIMES
			if (r->BVHListNode == -1)
				continue;
			
			qint64 elapsed = t.elapsed();
			totalTime += elapsed;
			i++;
		}

		std::ofstream measures;
		measures.open(measureFileName, std::ios::app);
		measures << totalTime / nrOfIterations << " & " ;
		measures.close();
#endif
	}
	break;
	case Qt::RightButton:
	{
		// Moving the view
		dragStartPosition = event->pos();
		mousePressed = true;
	}
	break;
	case Qt::MiddleButton:
	{
		rotateMovement = true;
		lastRotPoint = currenRotPoint = event->pos();
	}
	break;
	}
}

void OpenGlWidget3D::mouseReleaseEvent(QMouseEvent * e)
{
	if (!render)
		return;

	rotateMovement = false;
}



void OpenGlWidget3D::mouseMoveEvent(QMouseEvent * e)
{
	if (!render)
		return;
	/*
	QVector3D dir, currPoint;
	float rotAngle, zoomFactor;
	*/

	if (rotateMovement)
	{
		currenRotPoint = e->pos();// trackBallMapping(e->pos());
		/*
		dir = currPoint - lastRotPoint;
		float velocity = dir.length();
		if (velocity > 0.0001)
		{
			QVector3D rotAxis;
			rotAxis = QVector3D::crossProduct(lastRotPoint, currPoint);
			rotAngle = velocity * 40; // multiplied by constant determining the rotation sensitivity
			render->view.rotate(rotAngle, rotAxis.x(), rotAxis.y(), rotAxis.z());

			//render->cam.dir = QVector3D(render->view.transposed() * QVector4D(0, 0, -1, 0)).normalized();
			//render->cam.pos = QVector3D(render->view * QVector4D(0, 0, 0, 1));
			//qDebug() << render->cam.dir;
		}
		*/
	}

	//lastRotPoint = currPoint;

}

