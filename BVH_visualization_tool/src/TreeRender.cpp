#include "TreeRender.h"

#define EXPORT

void TreeRender::generateScalarSets()
{
	// area relatively to depth
	ScalarSet *a = new ScalarSet();
	a->name = "relative area";
	a->colors.resize(bvh->mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < bvh->mMeshCenterCoordinatesNr; i++)
	{
		a->colors[i] = bvh->mNodes[bvh->mMeshToBVHIndices[i]].GetBoxSize() *
					   pow(2,bvh->mNodeDepths[bvh->mMeshToBVHIndices[i]]);
	}
	bvh->mScalarSets.push_back(a);

	ScalarSet *b = new ScalarSet();
	b->name = "triangle number";
	b->colors.resize(bvh->mMeshCenterCoordinatesNr);
	for (unsigned i = 0; i < bvh->mMeshCenterCoordinatesNr; i++)
	{
		b->colors[i] = bvh->getTriangleCount(bvh->mMeshToBVHIndices[i]);
	}
	bvh->mScalarSets.push_back(b);
}

void TreeRender::removeScalarSets()
{
	for (std::vector<ScalarSet*>::iterator it = bvh->mScalarSets.begin(); it != bvh->mScalarSets.end(); ++it)
	{
		delete *it;
	}
	bvh->mScalarSets.clear();
}

void TreeRender::exportColors(const string &fileName)
{
	std::ofstream outFile(fileName, ios::binary);

	if (!outFile)
		throw "Output file not created";

	const uint32_t actualNodeSize = static_cast<uint32_t>(bvh->mMeshCenterCoordinatesNr);
	outFile.write(reinterpret_cast<const char*>(&actualNodeSize), sizeof(uint32_t));
	const uint8_t colorSetsSize = static_cast<uint8_t>(bvh->mScalarSets.size() + 1);
	outFile.write(reinterpret_cast<const char*>(&colorSetsSize), sizeof(uint8_t));

	string setName = "area";
	size_t len = 4;
	outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
	outFile.write(reinterpret_cast<const char*>(setName.c_str()), len);
	outFile.write(reinterpret_cast<const char*>(bvh->mBoxSizes.data()),
		actualNodeSize * sizeof(float));

	for (unsigned i = 0; i < colorSetsSize - 1; i++)
	{
		string setName = bvh->mScalarSets[i]->name;
		size_t len = setName.length();
		outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
		outFile.write(reinterpret_cast<const char*>(setName.c_str()), len);
		outFile.write(reinterpret_cast<const char*>(bvh->mScalarSets[i]->colors.data()),
			actualNodeSize * sizeof(float));
	}

	outFile.close();
}


TreeRender::TreeRender() : Render(RenderType::Tree)
{
	cam.pos = QVector3D(0, 0, 1);
	cam.dir = QVector3D(0, 0, -1);
	cam.upVector = QVector3D(0, 1, 0);

	model.setToIdentity();
	view.lookAt(cam.pos, cam.pos + cam.dir, cam.upVector);
	projection.setToIdentity();

	if (!initShaders(&shader, "src/shaders/vs.vert", "src/shaders/fs.frag"))
		throw "shader creation failed";

	drawer = NULL;
}

TreeRender::TreeRender(const string &sceneName) : Render(RenderType::Tree, sceneName)
{
	cam.pos = QVector3D(0, 0, 1);
	cam.dir = QVector3D(0, 0, -1);
	cam.upVector = QVector3D(0, 1, 0);

	model.setToIdentity();
	view.lookAt(cam.pos, cam.pos + cam.dir, cam.upVector);
	projection.setToIdentity();

	if (!initShaders(&shader, "src/shaders/vs.vert", "src/shaders/fs.frag"))
		throw "shader creation failed";

	drawer = new BVHDrawer(bvh, &shader);

#ifdef EXPORT
	generateScalarSets();
	exportColors("default.scal");
	removeScalarSets();
#endif // EXPORT

	sceneImporter->loadScalars("default.scal");
	drawer->changeScalarSet(0);
}

void TreeRender::loadScene(const string & sceneName)
{
	sc = new Scene();
	sceneImporter = new SceneImporter(bvh, sc);
	sceneImporter->loadFromBinaryFile(sceneName);
	drawer = new BVHDrawer(bvh, &shader);

#ifdef EXPORT
	generateScalarSets();
	exportColors("data/scalars_bin");
	removeScalarSets();
#endif // EXPORT

	sceneImporter->loadScalars("data/scalars_bin");
	bvh->normalizeScalarSets();
	drawer->changeScalarSet(0);
}

TreeRender::~TreeRender()
{
	delete drawer;
	drawer = NULL;
}

void TreeRender::draw()
{
	if (drawer) 
	{
		shader.bind();
		//assert(glGetError() == GL_NO_ERROR);
		shader.setUniformValue("mvp_matrix", projection * view * model);
		//assert(glGetError() == GL_NO_ERROR);
		drawer->draw();
	}
}

int TreeRender::pick(const QVector2D &point)
{
	for (int i = 0, s = bvh->meshExtremes.size(); i < s; i++)
	{
		QVector2D min = QVector2D(bvh->meshExtremes[i].extremes[0].x(), bvh->meshExtremes[i].extremes[0].y());
		QVector2D max = QVector2D(bvh->meshExtremes[i].extremes[1].x(), bvh->meshExtremes[i].extremes[1].y());
		if (point.x() >= min.x() && point.y() >= min.y() &&
			point.x() <= max.x() && point.y() <= max.y())
			return bvh->mMeshToBVHIndices[i];
	}
	return -1;
}

void TreeRender::displayPath(const std::vector<unsigned>& indices)
{
	drawer->clearPath();
	drawer->highlightNodes(indices);
}
