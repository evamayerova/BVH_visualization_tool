#include "SceneRender.h"



SceneRender::SceneRender() : Render(RenderType::SceneView)
{
	currentCamera = 0;
	Camera *cam = new Camera();
	cam->pos = QVector3D(0, 0, 1);
	cam->dir = QVector3D(0, 0, -1);
	cam->upVector = QVector3D(0, 1, 0);
	cams.push_back(cam);

	currCam.pos = QVector3D(cams[currentCamera]->pos);
	currCam.dir = QVector3D(cams[currentCamera]->dir);
	currCam.upVector = QVector3D(cams[currentCamera]->upVector);

	model.setToIdentity();
	view.lookAt(currCam.pos, currCam.pos + currCam.dir, currCam.upVector);

	if (!initShaders(&shader, "src/shaders/scene.vert", "src/shaders/scene.frag"))
		throw "shader creation failed";

	if (!initShaders(&bboxShader, "src/shaders/bbox.vert", "src/shaders/bbox.frag"))
		throw "shader creation failed";

	drawer = NULL;
}

void SceneRender::switchCamera(int camIndex)
{
	assert(camIndex < cams.size() && camIndex >= 0);
	currentCamera = camIndex;
	currCam.pos = cams[currentCamera]->pos;
	currCam.dir = cams[currentCamera]->dir;
	currCam.upVector = cams[currentCamera]->upVector;
	model.setToIdentity();
	view.setToIdentity();
	view.lookAt(currCam.pos, currCam.pos + currCam.dir, currCam.upVector);
}

void SceneRender::loadCameras(const string &camFilePath)
{
	ifstream in(camFilePath);
	if (!in)
	{
		Camera *cam = new Camera();
		cam->pos = QVector3D(0, 0, 1);
		cam->dir = QVector3D(0, 0, -1);
		cam->upVector = QVector3D(0, 1, 0);
		cams.push_back(cam);
		return;
	}

	string line;
	while (getline(in, line))
	{
		Camera *cam = new Camera();
		sscanf(line.c_str(), "(%f, %f, %f) (%f, %f, %f) (%f, %f, %f)",
			&cam->pos[0], &cam->pos[1], &cam->pos[2],
			&cam->dir[0], &cam->dir[1], &cam->dir[2],
			&cam->upVector[0], &cam->upVector[1], &cam->upVector[2]);
		cams.push_back(cam);
		qDebug() << QString::fromStdString(line);
	}
	
}

void SceneRender::loadLights(const string &lightsFilePath)
{
	ifstream in(lightsFilePath);
	if (!in)
	{
		light.position = QVector3D(1.f, 1.f, 1.f);
		light.diffuse = QVector3D(1.f, 1.f, 1.f);
		return;
	}

	string line;
	while (getline(in, line))
	{
		sscanf(line.c_str(), "%f %f %f %f %f %f",
			&light.position[0], &light.position[1], &light.position[2],
			&light.diffuse[0], &light.diffuse[1], &light.diffuse[2]);
		qDebug() << QString::fromStdString(line);
	}

}

SceneRender::SceneRender(
	const string &sceneName, 
	const string &camFile,
	const string &lightsFile) 
	: Render(RenderType::SceneView, sceneName)
{
	currentCamera = 0;
	loadCameras(camFile);
	loadLights(lightsFile);
	currCam.pos = QVector3D(cams[currentCamera]->pos);
	currCam.dir = QVector3D(cams[currentCamera]->dir);
	currCam.upVector = QVector3D(cams[currentCamera]->upVector);

	model.setToIdentity();
	view.lookAt(currCam.pos, currCam.pos + currCam.dir, currCam.upVector);

	if (!initShaders(&shader, "src/shaders/scene.vert", "src/shaders/scene.frag"))
		throw "shader creation failed";

	if (!initShaders(&bboxShader, "src/shaders/bbox.vert", "src/shaders/bbox.frag"))
		throw "shader creation failed";

	drawer = new SceneDrawer(sc, &shader, &bboxShader);
}

void SceneRender::loadScene(const string & sceneName)
{
	sc = new Scene();
	sceneImporter = new SceneImporter(bvhs[currentBVHIndex], sc);
	sceneImporter->loadFromBinaryFile(sceneName);
	drawer = new SceneDrawer(sc, &shader, &bboxShader);
}

SceneRender::~SceneRender()
{
	delete drawer;
	drawer = NULL;

	for (std::vector<Camera*>::iterator it = cams.begin(); it != cams.end(); it++)
	{
		delete *it;
		*it = NULL;
	}

	delete bvhs[0];
	bvhs[0] = NULL;
	bvhs.clear();
}

void SceneRender::draw()
{
	if (drawer)
	{
		drawer->draw(&projection, &view, &model, &light);
	}
}

int SceneRender::pick(ray &r)
{
	if (castRay(&bvhs[currentBVHIndex]->mNodes[0], 0, r))
		return 1;

	return 0;
}

void SceneRender::scrollView(int numSteps)
{
	qDebug() << numSteps;
	currCam.pos += numSteps * 0.1f * currCam.dir;
	view.setToIdentity();
	view.lookAt(currCam.pos, currCam.pos + currCam.dir, currCam.upVector);
}

const bool SceneRender::castRay(BVHNode *node, unsigned nodeIndex, ray & r)
{
	if (node->intersect(r)) {
		if (node->axis < 3) {
			bool left = castRay(&bvhs[currentBVHIndex]->mNodes[node->child], node->child, r);
			bool right = castRay(&bvhs[currentBVHIndex]->mNodes[node->child + 1], node->child + 1, r);
			return left || right;
		}
		else {
			drawer->setBBoxVertices(bvhs[currentBVHIndex], node);
			for (unsigned i = 0; i < node->children; i++) {
				if (rayTriangle(r, &sc->mTriangles[sc->mTriangleIdx[currentBVHIndex][node->child + i]]))
					r.BVHListNode = nodeIndex;
			}
		}
	}
	return r.t < std::numeric_limits<float>::infinity();
}

const bool SceneRender::rayTriangle(ray & r, Triangle * t)
{
	const QVector3D v0 = t->vertices[0];
	const QVector3D v1 = t->vertices[1];
	const QVector3D v2 = t->vertices[2];

	QVector3D e1 = v1 - v0;
	QVector3D e2 = v2 - v0;
	QVector3D pVec = QVector3D::crossProduct(r.dir, e2);
	float det = QVector3D::dotProduct(e1, pVec);
	if (det == 0)
		return false;
	float invDet = 1 / det;
	QVector3D tVec = r.orig - v0;
	float u = QVector3D::dotProduct(tVec, pVec) * invDet;
	if (u < 0 || u > 1)
		return false;
	QVector3D qVec = QVector3D::crossProduct(tVec, e1);
	float v = QVector3D::dotProduct(r.dir, qVec) * invDet;
	if (v < 0 || u + v > 1)
		return false;
	float tNew = QVector3D::dotProduct(e2, qVec) * invDet;

	if (tNew < r.t)
	{
		r.t = tNew;
		r.norm = QVector3D::crossProduct(e1, e2).normalized();
		return true;
	}
	return false;
}

void SceneRender::addBBox(BVHNode * n)
{
	drawer->setBBoxVertices(bvhs[currentBVHIndex], n);
}

void SceneRender::removeBBox()
{
	drawer->unsetBBoxMesh();
}
