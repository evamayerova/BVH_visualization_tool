
#include "TrackballCamera.h"

const float TrackballCamera::INITIAL_FOV = 30;
const float TrackballCamera::TRANSLATION_FACTOR = 0.01f;

TrackballCamera::TrackballCamera()
{
	this->ballRadius = 600;
	isRotating = false;
	width = height = 0;
	reset();
}

void TrackballCamera::startRotation(int x, int y)
{
	int a = ((x)-(width / 2));
	int b = ((height / 2) - y);

	startRotationVector = mapCoordinatesToSphere(a, b);
	startRotationVector.normalize();

	currentRotationVector = startRotationVector;
	isRotating = true;
}

void TrackballCamera::updateRotation(int x, int y)
{
	int a = ((x)-(width / 2));
	int b = ((height / 2) - y);

	currentRotationVector = mapCoordinatesToSphere(x, y);

	currentRotationVector.normalize();
}

void TrackballCamera::rotate()
{
	if (isRotating)
	{  // Do some rotation according to start and current rotation vectors
	   //cerr << currentRotationVector.transpose() << " " << startRotationVector.transpose() << endl;
		if ((currentRotationVector - startRotationVector).length() > 1E-6)
		{
			QVector3D rotationAxis = QVector3D::crossProduct(currentRotationVector, startRotationVector);
			rotationAxis.normalize();

			double val = QVector3D::dotProduct(currentRotationVector, startRotationVector);
			val > (1 - 1E-10) ? val = 1.0 : val = val;
			double rotationAngle = acos(val) * 180.0f / (float)M_PI;

			// rotate around the current position
			translate(true);
			currentMatrix.rotate(rotationAngle * 2, -rotationAxis);
			//glRotatef(rotationAngle * 2, -rotationAxis.x(), -rotationAxis.y(), -rotationAxis.z());
			translate(false);
		}
	}
	currentMatrix *= startMatrix;
}

void TrackballCamera::stopRotation()
{
	startMatrix = currentMatrix;
	isRotating = false;
}

void TrackballCamera::translate(bool reverse)
{
	float factor = (reverse ? -1.0f : 1.0f);
	float tx = transX + (currentTransX - startTransX)*TRANSLATION_FACTOR;
	float ty = transY + (currentTransY - startTransY)*TRANSLATION_FACTOR;
	currentMatrix.translate(factor*tx, factor*(-ty), 0);
}

QVector3D TrackballCamera::mapCoordinatesToSphere(int x, int y)
{
	int d = x*x + y*y;
	float radiusSquared = ballRadius*ballRadius;
	if (d > radiusSquared)
	{
		return QVector3D((float)x, (float)y, 0);
	}
	else
	{
		return QVector3D((float)x, (float)y, sqrt(radiusSquared - d));
	}
}

void TrackballCamera::setDimensions(int w, int h)
{
	width = w;
	height = h;
	ballRadius = min((int)(w / 2), (int)(h /2));
}

void TrackballCamera::setRadius(const float & r)
{
	ballRadius = r;
}

void TrackballCamera::reset()
{
	fov = INITIAL_FOV;

	startMatrix.setToIdentity();

	transX = transY = 0;
	startTransX = startTransY = currentTransX = currentTransY = 0;
}
