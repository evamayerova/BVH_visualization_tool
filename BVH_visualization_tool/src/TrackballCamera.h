
#define _USE_MATH_DEFINES

#include <QVector3D>
#include <QMatrix4x4>
#include <algorithm>
#include <cmath>

using namespace std;

class TrackballCamera
{
private:
	float fov;
	int fovStartY;
	int fovCurrentY;

	float transX, transY;
	float currentTransX, currentTransY;
	float startTransX, startTransY;

	QMatrix4x4 startMatrix;
	QMatrix4x4 currentMatrix;
	QVector3D startRotationVector;
	QVector3D currentRotationVector;
	bool isRotating;
	float ballRadius;
	double residualSpin;
	static const float INITIAL_FOV;
	static const float MINIMAL_FOV;
	static const float TRANSLATION_FACTOR;

	QVector3D mapCoordinatesToSphere(int x, int y);
	int width, height;

public:
	TrackballCamera();
	void startRotation(int x, int y);
	void updateRotation(int x, int y);
	void rotate();
	void stopRotation();
	void translate(bool reverse);
	void setDimensions(int w, int h);
	void setRadius(const float &r);
	void reset();
	QMatrix4x4 getCurrentMatrix()
	{
		return currentMatrix;
	}

};
