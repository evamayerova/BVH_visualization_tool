#include "BVHDrawer.h"

#include <time.h>

#define MAX_MESH_WIDTH 0.2

//#define DRAW_ONLY_TEXTURES

BVHDrawer::BVHDrawer(BVH *b, QOpenGLShaderProgram *sp, QOpenGLShaderProgram *val, QOpenGLShaderProgram *counts, int treeDepth)
{
	initializeOpenGLFunctions();
	shaderProgram = sp;
	bvh = b;
	bvh->generateTree(treeDepth);
	generateMeshes();
	currentScalarSet = 0;
	currentNode = -1;
	textureRenderCountsShader = counts;
	textureRenderValuesShader = val;

	texturesInitialized = false;
}

BVHDrawer::~BVHDrawer()
{
	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		delete *it;
		*it = NULL;
	}

	bvh = NULL;

	shaderProgram = NULL;
	textureRenderCountsShader = NULL;
	textureRenderValuesShader = NULL;

	textureRenderCountsShader = NULL;
	textureRenderValuesShader = NULL;

	glDeleteTextures(1, &textureCounts);
	glDeleteTextures(1, &textureValues);

	glDeleteFramebuffers(1, &fboCounts);
	glDeleteFramebuffers(1, &fboValues);
}

void BVHDrawer::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
	shaderProgram->bind();
	
	for (int i = 0, s = meshes.size(); i < s; i++)
	{
		switch (blendMode) {
		case maxVal:
			textureRenderValuesShader->bind();
			textureRenderValuesShader->setUniformValue("scalar_min", bvh->mScalarSets[currentScalarSet]->selectedMin);
			textureRenderValuesShader->setUniformValue("scalar_max", bvh->mScalarSets[currentScalarSet]->selectedMax);

#ifndef DRAW_ONLY_TEXTURES
			glBindFramebuffer(GL_FRAMEBUFFER, fboValues);
#endif
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendEquation(GL_MAX);
			meshes[i]->drawArraysPoints();
			glDisable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureValues);
			break;

		case minVal:
			textureRenderValuesShader->bind();
			textureRenderValuesShader->setUniformValue("scalar_min", bvh->mScalarSets[currentScalarSet]->selectedMin);
			textureRenderValuesShader->setUniformValue("scalar_max", bvh->mScalarSets[currentScalarSet]->selectedMax);

#ifndef DRAW_ONLY_TEXTURES
			glBindFramebuffer(GL_FRAMEBUFFER, fboValues);
			//glBindTexture(GL_TEXTURE_2D, textureValues);
#endif
			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendEquation(GL_MIN);
			meshes[i]->drawArraysPoints();
			glDisable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureValues);
			break;

		case aveVal:
			// first pass - render sum of values 
			
#ifndef DRAW_ONLY_TEXTURES
			glBindFramebuffer(GL_FRAMEBUFFER, fboValues);
#endif
			textureRenderValuesShader->bind();
			textureRenderValuesShader->setUniformValue("scalar_min", bvh->mScalarSets[currentScalarSet]->selectedMin);
			textureRenderValuesShader->setUniformValue("scalar_max", bvh->mScalarSets[currentScalarSet]->selectedMax);

			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			meshes[i]->drawArraysPoints();
			glDisable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureValues);
			
			// second pass - render counts of pixel candidates
			/*
#ifndef DRAW_ONLY_TEXTURES
			glBindFramebuffer(GL_FRAMEBUFFER, fboCounts);
#endif
			textureRenderCountsShader->bind();

			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			meshes[i]->drawArraysPoints();
			glDisable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureCounts);
			*/
			break;

		case topVal:
			textureRenderValuesShader->bind();
			textureRenderValuesShader->setUniformValue("scalar_min", bvh->mScalarSets[currentScalarSet]->selectedMin);
			textureRenderValuesShader->setUniformValue("scalar_max", bvh->mScalarSets[currentScalarSet]->selectedMax);

#ifndef DRAW_ONLY_TEXTURES
			glBindFramebuffer(GL_FRAMEBUFFER, fboValues);
			//glBindTexture(GL_TEXTURE_2D, textureValues);
#endif
			glClearColor(0.f, 0.f, 0.f, 1.f);
			meshes[i]->drawArraysPoints();

			glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, textureValues);
			break;

		}

#ifndef DRAW_ONLY_TEXTURES
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram->bind();
		shaderProgram->setUniformValue("colorMapMin", 0.f);
		shaderProgram->setUniformValue("colorMapMax", 1.f);
		meshes[i]->drawArraysPoints();

		//glBindTexture(GL_TEXTURE_2D, 0);
#endif
	}
}

void BVHDrawer::clearPath()
{
	highlightedNodes.clear();
	currentNode = -1;
	changeScalarSet(currentScalarSet);
}

void BVHDrawer::setBlendMode(int mode)
{
	switch (mode) {
	case 0:
		blendMode = maxVal;
		break;
	case 1:
		blendMode = minVal;
		break;
	case 2:
		blendMode = aveVal;
		break;
	case 3:
		blendMode = topVal;
		break;
	}
}

void BVHDrawer::setShaderProgram(QOpenGLShaderProgram * sp)
{
	shaderProgram = sp;
}

void BVHDrawer::reshape(const QSize & s)
{
	screenSize.setWidth(s.width());
	screenSize.setHeight(s.height());
	
	if (!texturesInitialized)
	{
		initScalarTexture(1, fboValues, textureValues);
		initScalarTexture(2, fboCounts, textureCounts);
		texturesInitialized = true;
	}
	
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFrameBuffer);
	reshapeScalarTexture(textureValues);
	reshapeScalarTexture(textureCounts);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);

	/*
	glBindRenderbuffer(GL_RENDERBUFFER, textureCounts);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, screenSize.width(), screenSize.height());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, textureValues);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, screenSize.width(), screenSize.height());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	*/
}


void BVHDrawer::initScalarTexture(int i, GLuint & fbo, GLuint & tex)
{
	glActiveTexture(GL_TEXTURE0 + i);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		screenSize.width(), 
		screenSize.height(),
		0,
		GL_RGB,
		GL_FLOAT,
		NULL);
	glBindTexture(GL_TEXTURE_2D, screenFrameBuffer);
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		tex, 
		0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "frame buffer not complete";
}

void BVHDrawer::reshapeScalarTexture(GLuint & tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB,
		screenSize.width(), 
		screenSize.height(),
		0,
		GL_RGB,
		GL_FLOAT,
		NULL);
	glBindTexture(GL_TEXTURE_2D, screenFrameBuffer);
}

void BVHDrawer::generateMeshes()
{
	meshes.clear();
	if (bvh->mMeshCenterCoordinatesNr < 2)
		return;

	bvh->meshExtremes.clear();
	GLfloat meshHeight = 1.0 / float(bvh->depth);
	GLfloat meshWidth = 1.0;
	GLfloat actualDepth = bvh->mMeshCenterCoordinates[1];
	float maxBoxSize = bvh->mNodes[0].GetBoxArea();

	std::vector<GLfloat> meshVertices;
	std::vector<unsigned int> meshIndices;
	std::vector<float> widths, heights;

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
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i]);
		meshVertices.push_back(bvh->mMeshCenterCoordinates[i + 1]);
		meshVertices.push_back(0);

		widths.push_back(resizedMeshWidth);
		heights.push_back(meshHeight);

		/*
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
		*/

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
	//m->indicesNr = meshIndices.size();
	m->indicesNr = bvh->mMeshCenterCoordinatesNr;

	glGenVertexArrays(1, &m->vao);
	assert(glGetError() == GL_NO_ERROR);
	glBindVertexArray(m->vao);
	assert(glGetError() == GL_NO_ERROR);

	// node center position
	glGenBuffers(1, &m->vbo);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * meshVertices.size(), meshVertices.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int vertexLoc = shaderProgram->attributeLocation("position");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(0);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	// half node width
	glGenBuffers(1, &m->vboW);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vboW);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * widths.size(), widths.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	//int halfWidthLoc = shaderProgram->attributeLocation("half_node_width");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(1);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	// half node height
	glGenBuffers(1, &m->vboH);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vboH);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * heights.size(), heights.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	//int halfHeightLoc = shaderProgram->attributeLocation("half_node_height");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(2);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	/*
	glGenBuffers(1, &m->eao);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->eao);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);
	*/

	glGenBuffers(1, &m->vboC);
	assert(glGetError() == GL_NO_ERROR);
	glBindBuffer(GL_ARRAY_BUFFER, m->vboC);
	assert(glGetError() == GL_NO_ERROR);
	glBufferData(GL_ARRAY_BUFFER, bvh->mMeshCenterCoordinatesNr * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, bvh->mNodes.size() * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int colorLoc = shaderProgram->attributeLocation("color_in");
	assert(glGetError() == GL_NO_ERROR);
	shaderProgram->enableAttributeArray(3);
	assert(glGetError() == GL_NO_ERROR);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	meshes.push_back(m);
}

#define SHORT

void BVHDrawer::changeScalarSet(int index)
{
	this->currentScalarSet = index;

	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	glBufferSubData(
		GL_ARRAY_BUFFER,
		0,
		bvh->mScalarSets[index]->colors.size() * sizeof(float),
		bvh->mScalarSets[index]->colors.data());
}

void BVHDrawer::highlightNodes(const vector<unsigned> &indices)
{
	this->highlightedNodes = indices;

	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	float white = -1.f;
	for (vector<unsigned>::const_iterator it = indices.begin(); it != indices.end(); it++)
	{
		glBufferSubData(
			GL_ARRAY_BUFFER,
			bvh->mBVHToMeshIndices[*it] * 4,
			sizeof(float),
			&white);
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

	float white = -1.f;
	glBindVertexArray(meshes[0]->vao);
	glBindBuffer(GL_ARRAY_BUFFER, meshes[0]->vboC);
	glBufferSubData(
		GL_ARRAY_BUFFER,
		bvh->mBVHToMeshIndices[index] * 4,
		1 * sizeof(float),
		&white);
	//glBufferSubData(GL_ARRAY_BUFFER, bvh->mBVHToMeshIndices[index] * 4 * meshColors.size(), sizeof(float) * meshColors.size(), meshColors.data());

}
