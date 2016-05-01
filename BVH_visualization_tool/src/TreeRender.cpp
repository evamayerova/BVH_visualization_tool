#include "TreeRender.h"

//#define EXPORT

void TreeRender::removeScalarSets()
{
	for (std::vector<ScalarSet*>::iterator it = bvhs[currentBVHIndex]->mScalarSets.begin(); it != bvhs[currentBVHIndex]->mScalarSets.end(); ++it)
	{
		delete *it;
	}
	bvhs[currentBVHIndex]->mScalarSets.clear();
}

void TreeRender::exportColors(const string &fileName)
{
	std::ofstream outFile(fileName, ios::binary);

	if (!outFile)
		throw "Output file not created";

	const uint32_t actualNodeSize = static_cast<uint32_t>(bvhs[currentBVHIndex]->mMeshCenterCoordinatesNr);
	outFile.write(reinterpret_cast<const char*>(&actualNodeSize), sizeof(uint32_t));
	const uint8_t colorSetsSize = static_cast<uint8_t>(bvhs[currentBVHIndex]->mScalarSets.size() + 1);
	outFile.write(reinterpret_cast<const char*>(&colorSetsSize), sizeof(uint8_t));

	/*
	string setName = "area";
	size_t len = 4;
	outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
	outFile.write(reinterpret_cast<const char*>(setName.c_str()), len);
	outFile.write(reinterpret_cast<const char*>(bvhs[currentBVHIndex]->mBoxSizes.data()),
		actualNodeSize * sizeof(float));
	*/

	for (unsigned i = 0; i < colorSetsSize - 1; i++)
	{
		string setName = bvhs[currentBVHIndex]->mScalarSets[i]->name;
		size_t len = setName.length();
		outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
		outFile.write(reinterpret_cast<const char*>(setName.c_str()), len);
		outFile.write(reinterpret_cast<const char*>(bvhs[currentBVHIndex]->mScalarSets[i]->colors.data()),
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

	if (!initShaders(&shader, "src/shaders/conservative.vert", "src/shaders/conservative.frag", "src/shaders/conservative.geom"))
		throw "shader creation failed";

}

TreeRender::TreeRender(const string &sceneName) : Render(RenderType::Tree, sceneName)
{
	cam.pos = QVector3D(0, 0, 1);
	cam.dir = QVector3D(0, 0, -1);
	cam.upVector = QVector3D(0, 1, 0);

	model.setToIdentity();
	view.lookAt(cam.pos, cam.pos + cam.dir, cam.upVector);
	projection.setToIdentity();

	if (!initShaders(&solidBoxShader, "src/shaders/conservative.vert", "src/shaders/conservative.frag", "src/shaders/conservative_triangles.geom"))
		throw "shader creation failed";

	if (!initShaders(&lineBoxShader, "src/shaders/conservative.vert", "src/shaders/conservative.frag", "src/shaders/conservative_lines.geom"))
		throw "shader creation failed";

	/*
	if (!initShaders(&solidRingShader, "src/shaders/conservative.vert", "src/shaders/conservative_rings.frag", "src/shaders/conservative.geom"))
		throw "shader creation failed";

	if (!initShaders(&lineRingShader, "src/shaders/conservative.vert", "src/shaders/conservative.frag", "src/shaders/conservative.geom"))
		throw "shader creation failed";
		*/

	currentShader = &solidBoxShader;

	BVHDrawer *drawer = new BVHDrawer(bvhs[currentBVHIndex], currentShader);

#ifdef EXPORT
	bvhs[currentBVHIndex]->setDefaultScalars();
	exportColors("default.scal");
	removeScalarSets();
	sceneImporter->loadScalars("default.scal");
#else
	bvhs[currentBVHIndex]->setDefaultScalars();
#endif // EXPORT

	drawer->changeScalarSet(0);
	drawers.push_back(drawer);
}

void TreeRender::loadScene(const string & sceneName)
{
	sc = new Scene();
	sceneImporter = new SceneImporter(bvhs[currentBVHIndex], sc);
	sceneImporter->loadFromBinaryFile(sceneName);
	BVHDrawer *drawer = new BVHDrawer(bvhs[currentBVHIndex], currentShader);

#ifdef EXPORT
	generateScalarSets();
	exportColors("data/scalars_bin");
	removeScalarSets();
#endif // EXPORT

	sceneImporter->loadScalars("data/scalars_bin");
	bvhs[currentBVHIndex]->normalizeScalarSets();
	drawer->changeScalarSet(0);
	drawers.push_back(drawer);
}

bool TreeRender::addBVH(const string & fileName)
{
	BVH *bvh = new BVH();
	if (sceneImporter->loadBVH(bvh, fileName))
	{
		currentBVHIndex = bvhs.size();

		BVHDrawer *drawer = new BVHDrawer(bvh, currentShader);
		drawers.push_back(drawer);

		bvh->setDefaultScalars();
		bvh->normalizeScalarSets();
		drawer->changeScalarSet(0);
		bvhs.push_back(bvh);
		return true;
	}
	return false;
}

void TreeRender::changeTreeDepth(int newDepth, int scalarSet)
{
	delete drawers[currentBVHIndex];
	drawers[currentBVHIndex] = new BVHDrawer(bvhs[currentBVHIndex], currentShader, newDepth);
	bvhs[currentBVHIndex]->setDefaultScalars();
	bvhs[currentBVHIndex]->normalizeScalarSets();
	drawers[currentBVHIndex]->changeScalarSet(scalarSet);
}

TreeRender::~TreeRender()
{
	for (vector<BVHDrawer*>::iterator it = drawers.begin(); it != drawers.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
	drawers.clear();
}

void TreeRender::draw()
{
	if (currentBVHIndex < drawers.size())
	{
		currentShader->bind();
		//assert(glGetError() == GL_NO_ERROR);
		currentShader->setUniformValue("mvp_matrix", projection * view * model);
		currentShader->setUniformValue("hPixel", hPixel);
		//assert(glGetError() == GL_NO_ERROR);
		drawers[currentBVHIndex]->draw();
	}
}

int TreeRender::pick(const QVector2D &point)
{
	for (int i = 0; i < bvhs[currentBVHIndex]->mMeshCenterCoordinatesNr; i++)
	{
		QVector2D min = QVector2D(bvhs[currentBVHIndex]->meshExtremes[i].extremes[0].x(), bvhs[currentBVHIndex]->meshExtremes[i].extremes[0].y());
		QVector2D max = QVector2D(bvhs[currentBVHIndex]->meshExtremes[i].extremes[1].x(), bvhs[currentBVHIndex]->meshExtremes[i].extremes[1].y());
		if (point.x() >= min.x() && point.y() >= min.y() &&
			point.x() <= max.x() && point.y() <= max.y())
			return bvhs[currentBVHIndex]->mMeshToBVHIndices[i];
	}
	return -1;
}

void TreeRender::displayPath(const std::vector<unsigned>& indices)
{
	drawers[currentBVHIndex]->clearPath();
	drawers[currentBVHIndex]->highlightNodes(indices);
}

void TreeRender::changeCurrentShader(int current)
{
	switch (current) {
	case 0:
		currentShader = &solidBoxShader;
		break;
	case 1:
		currentShader = &lineBoxShader;
		break;
	case 2:
		//currentShader = &solidRingShader;
		break;
	case 3:
		//currentShader = &lineRingShader;
		break;
	}

	drawers[currentBVHIndex]->setShaderProgram(currentShader);
}

void TreeRender::changeCurrentBVH(int current)
{
	currentBVHIndex = current;
	drawers[currentBVHIndex]->setShaderProgram(currentShader);
}
