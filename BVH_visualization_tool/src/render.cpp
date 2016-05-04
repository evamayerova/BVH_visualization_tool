#include "render.h"

Render::Render(RenderType::Type type)
{
    renderType = type;
    view.setToIdentity();
	model.setToIdentity();
    projection.setToIdentity();
	scaleFactor = 1;
	startingPosition = QVector3D(0.f, 0.f, 0.f);
	//bvh = new BVH();
}

#include <time.h>

void Render::exportColors(const string &fileName)
{
	std::ofstream outFile(fileName, ios::binary);
	
	if (!outFile)
		throw "Output file not created";

	const uint32_t actualNodeSize = static_cast<uint32_t>(bvhs[currentBVHIndex]->mMeshCenterCoordinatesNr);
	outFile.write(reinterpret_cast<const char*>(&actualNodeSize), sizeof(uint32_t));
	const uint8_t colorSetsSize = static_cast<uint8_t>(bvhs[currentBVHIndex]->mScalarSets.size());
	outFile.write(reinterpret_cast<const char*>(&colorSetsSize), sizeof(uint8_t));

	string area = "area"; outFile.write((char *)&area, sizeof(string));
	outFile.write(reinterpret_cast<const char*>(bvhs[currentBVHIndex]->mBoxSizes.data()),
		actualNodeSize * sizeof(float));

	for (unsigned i = 0; i < colorSetsSize; i++)
	{
		string setName = bvhs[currentBVHIndex]->mScalarSets[i]->name;
		outFile.write((char *)&setName, sizeof(string));
		outFile.write(reinterpret_cast<const char*>(bvhs[currentBVHIndex]->mScalarSets[i]->colors.data()),
			actualNodeSize * sizeof(float));
	}

	outFile.close();
}

Render::Render(RenderType::Type type, const string &sceneName)
{
    renderType = type;
    scaleFactor = 1;
	startingPosition = QVector3D(0.f, 0.f, 0.f);

    sc = new Scene();
    BVH *bvh = new BVH();
	bvhs.push_back(bvh);

	currentBVHIndex = 0;

	sceneImporter = new SceneImporter(bvh, sc);
	sceneImporter->loadFromBinaryFile(sceneName);
}

Render::~Render()
{
    delete sc;
    sc = NULL;

	delete sceneImporter;
	sceneImporter = NULL;
}
/*
void Render::moveView(const QVector3D &change)
{
	model.translate(change.x(), change.y(), 0.f);
	startingPosition[0] -= change.x() * scaleFactor;
	startingPosition[1] -= change.y();
	currentPosition = change;
}
*/
void Render::resetView()
{
	// update scaling
	model.scale(1 / this->scaleFactor, 1., 1.);
	// update translate
	model.translate(startingPosition[0], startingPosition[1], startingPosition[2]);

	scaleFactor = 1;
	startingPosition = QVector3D(0.f, 0.f, 0.f);
}

void Render::scaleView(const float &scaleFactor)
{
	if (scaleFactor == 1)
		return;
	
	QVector3D posRaw = -startingPosition / this->scaleFactor;
	moveView(-posRaw);

	model.scale(1/this->scaleFactor, 1., 1.);
	this->scaleFactor = std::max(0.1f, this->scaleFactor * (1 + scaleFactor));
	model.scale(this->scaleFactor, 1., 1.);

	moveView(posRaw);

}

int Render::initShaders(QOpenGLShaderProgram *shader_program, const char *vs, const char *fs, const char *gs)
{
    if (!shader_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vs))
        return 0;

	if (strcmp(gs, "") != 0)
		if (!shader_program->addShaderFromSourceFile(QOpenGLShader::Geometry, gs))
			return 0;

    if (!shader_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fs))
        return 0;

    if (!shader_program->link())
        return 0;

    if (!shader_program->bind())
        return 0;

    return 1;
}
