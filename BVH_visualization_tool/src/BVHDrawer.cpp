#include "BVHDrawer.h"

#include <time.h>

#define MAX_MESH_WIDTH 0.2

BVHDrawer::BVHDrawer(BVH *b, QOpenGLShaderProgram *sp)
{
	initializeOpenGLFunctions();
	shaderProgram = sp;
	bvh = b;
	bvh->generateTree();
	generateMeshes();
	currentScalarSet = 0;
	currentNode = -1;
}

BVHDrawer::~BVHDrawer()
{
}

void BVHDrawer::draw() const
{
	shaderProgram->bind();
	for (int i = 0, s = meshes.size(); i < s; i++)
	{
		shaderProgram->bind();
		meshes[i]->drawElements();
	}
}

void BVHDrawer::clearPath()
{
	highlightedNodes.clear();
	currentNode = -1;
	changeScalarSet(currentScalarSet);
}

void BVHDrawer::generateMeshes()
{
	meshes.clear();
	if (bvh->mMeshCenterCoordinatesNr < 2)
		return;

	GLfloat meshHeight = 1.0 / float(bvh->depth);
	GLfloat meshWidth = 1.0;
	GLfloat actualDepth = bvh->mMeshCenterCoordinates[1];
	float maxBoxSize = bvh->mNodes[0].GetBoxSize();

	std::vector<GLfloat> meshVertices;
	std::vector<unsigned int> meshIndices;

	unsigned mSize = 0;

	srand(time(NULL));

	for (int i = 0; i < bvh->mMeshCenterCoordinatesNr * 2; i += 2)
	{
		if (bvh->mMeshCenterCoordinates[i + 1] < actualDepth)
		{
			meshWidth /= bvh->arity;
			actualDepth = bvh->mMeshCenterCoordinates[i + 1];
		}

		float resizedMeshWidth = min(meshWidth, (float)MAX_MESH_WIDTH / 2.f);

		meshVertices.push_back(bvh->mMeshCenterCoordinates[i] - resizedMeshWidth);
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i + 1] - meshHeight);
		meshVertices.push_back(0.0);
	
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i] + resizedMeshWidth);
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i + 1] - meshHeight);
		meshVertices.push_back(0.0);

		meshVertices.push_back(bvh->mMeshCenterCoordinates[i] + resizedMeshWidth);
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i + 1] + meshHeight);
		meshVertices.push_back(0.0);

		meshVertices.push_back(bvh->mMeshCenterCoordinates[i] - resizedMeshWidth);
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i + 1] + meshHeight);
		meshVertices.push_back(0.0);

		meshIndices.push_back(mSize + 0);
		meshIndices.push_back(mSize + 1);
		meshIndices.push_back(mSize + 3);
		meshIndices.push_back(mSize + 1);
		meshIndices.push_back(mSize + 2);
		meshIndices.push_back(mSize + 3);

		BBox b;
		b.extremes[0] = QVector3D(bvh->mMeshCenterCoordinates[i] - resizedMeshWidth,
			bvh->mMeshCenterCoordinates[i + 1] - meshHeight,
			0.0);
		b.extremes[1] = QVector3D(bvh->mMeshCenterCoordinates[i] + resizedMeshWidth,
			bvh->mMeshCenterCoordinates[i + 1] + meshHeight,
			0.0);
		bvh->meshExtremes.push_back(b);

		mSize += 4;
	}

	Mesh *m = new Mesh(shaderProgram);
	m->indicesNr = meshIndices.size();

	glGenVertexArrays(1, &m->vao);
	assert(glGetError() == GL_NO_ERROR);
	glBindVertexArray(m->vao);
	assert(glGetError() == GL_NO_ERROR);

	glGenBuffers(1, &m->vbo);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * meshVertices.size(), meshVertices.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int vertexLoc = shaderProgram->attributeLocation("position");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(vertexLoc);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);


	glGenBuffers(1, &m->eao);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->eao);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	glGenBuffers(1, &m->vboC);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vboC);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, bvh->mNodes.size() * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int colorLoc = shaderProgram->attributeLocation("color_in");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(colorLoc);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	meshes.push_back(m);

	//changeScalarSet(0);

}

#define SHORT

void BVHDrawer::changeScalarSet(int index)
{
	currentScalarSet = index;
	std::vector<float> meshColors;
	colorMapping cm;

	for (int i = 0; i < bvh->mMeshCenterCoordinatesNr * 2; i += 2)
	{
#ifdef SHORT
		float val = bvh->mScalarSets[index]->colors[i / 2];
#else
		float val = bvh->mScalarSets[index]->colors[bvh->mMeshToBVHIndices[i / 2]];
#endif
		//float val = bvh->mNodes[ceil(i / 2.0)].GetBoxSize() / maxBoxSize * pow(2, (1 - actualDepth)*10);
		//float val = actualDepth;
		//float val = (int(f) % 10000) / 10000.;
		QVector3D res = cm.setRainbowColor(val, bvh->mScalarSets[index]->selectedMin, bvh->mScalarSets[index]->selectedMax) / 255.;
		for (int j = 0; j < 4; j++)
		{
			meshColors.push_back(res[0]);
			meshColors.push_back(res[1]);
			meshColors.push_back(res[2]);
		}
	}

	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	glBufferSubData(GL_ARRAY_BUFFER, 0, meshColors.size() * sizeof(float), meshColors.data());

}

void BVHDrawer::highlightNodes(const vector<unsigned> &indices)
{
	this->highlightedNodes = indices;
	std::vector<float> meshColors;
	for (int i = 0; i < 4; i++)
	{
		meshColors.push_back(1.0);
		meshColors.push_back(1.0);
		meshColors.push_back(1.0);
	}

	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	for (vector<unsigned>::const_iterator it = indices.begin(); it != indices.end(); it++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, bvh->mBVHToMeshIndices[*it] * 4 * meshColors.size(), sizeof(float) * meshColors.size(), meshColors.data());
	}
}

void BVHDrawer::showDisplayedNodes()
{
	if (currentNode != -1)
		highlightNode(currentNode);
	else
		if (highlightedNodes.size() > 0)
			highlightNodes(highlightedNodes);
}

void BVHDrawer::highlightNode(const unsigned &index)
{
	currentNode = index;
	std::vector<float> meshColors;
	for (int i = 0; i < 4; i++)
	{
		meshColors.push_back(1.0);
		meshColors.push_back(1.0);
		meshColors.push_back(1.0);
	}

	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	glBufferSubData(GL_ARRAY_BUFFER, bvh->mBVHToMeshIndices[index] * 4 * meshColors.size(), sizeof(float) * meshColors.size(), meshColors.data());

}
