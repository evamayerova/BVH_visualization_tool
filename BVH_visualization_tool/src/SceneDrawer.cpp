#include "SceneDrawer.h"
#include <algorithm>


SceneDrawer::SceneDrawer(Scene *sc, QOpenGLShaderProgram *sceneShader, QOpenGLShaderProgram *bboxShader)
{
	initializeOpenGLFunctions();

	scene = sc;
	this->sceneShader = sceneShader;
	this->bboxShader = bboxShader;
	sceneMesh = new Mesh(sceneShader);
	bboxMesh = new Mesh(bboxShader);
	setSceneBuffers();
	setBBoxBuffers();
	showBBox = false;
}


SceneDrawer::~SceneDrawer()
{
	delete sceneMesh;
	sceneMesh = NULL;

	delete bboxMesh;
	bboxMesh = NULL;

	scene = NULL;
	sceneShader = NULL;
	bboxShader = NULL;
}

void SceneDrawer::setSceneBuffers()
{
	QVector3D mmin, mmax;
	mmin = mmax = scene->mTriangles[0].vertices[0];

	std::vector<float> rawTriangles;
	rawTriangles.reserve(scene->mTriangles.size() * 9);

	std::vector<QVector3D> normals;
	normals.reserve(scene->mTriangles.size() * 3);
	
	sceneColors.reserve(scene->mTriangles.size() * 3);

	for (unsigned i = 0, s = scene->mTriangles.size(); i < s; i++) {
		QVector3D a, b, c;
		a = scene->mTriangles[i].vertices[0];
		b = scene->mTriangles[i].vertices[1];
		c = scene->mTriangles[i].vertices[2];

		normals.push_back(QVector3D::normal(a, b, c));
		normals.push_back(QVector3D::normal(b, c, a));
		normals.push_back(QVector3D::normal(c, a, b));

		for (int j = 0; j < 3; j++) {
			rawTriangles.push_back(scene->mTriangles[i].vertices[j][0]);
			rawTriangles.push_back(scene->mTriangles[i].vertices[j][1]);
			rawTriangles.push_back(scene->mTriangles[i].vertices[j][2]);

			mmin[0] = std::min(mmin[0], scene->mTriangles[i].vertices[j][0]);
			mmin[1] = std::min(mmin[1], scene->mTriangles[i].vertices[j][1]);
			mmin[2] = std::min(mmin[2], scene->mTriangles[i].vertices[j][2]);

			mmax[0] = std::max(mmax[0], scene->mTriangles[i].vertices[j][0]);
			mmax[1] = std::max(mmax[1], scene->mTriangles[i].vertices[j][1]);
			mmax[2] = std::max(mmax[2], scene->mTriangles[i].vertices[j][2]);
		}

		sceneColors.push_back(scene->mTriangles[i].color);
		sceneColors.push_back(scene->mTriangles[i].color);
		sceneColors.push_back(scene->mTriangles[i].color);
	}
	qDebug() << mmin << mmax;
	sceneMesh->indicesNr = scene->mTriangles.size();

	glGenVertexArrays(1, &sceneMesh->vao);
	glBindVertexArray(sceneMesh->vao);

	glGenBuffers(1, &sceneMesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sceneMesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rawTriangles.size(), rawTriangles.data(), GL_STATIC_DRAW);
	
	int vertexLoc = sceneShader->attributeLocation("position");
	sceneShader->enableAttributeArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	 
	glGenBuffers(1, &sceneMesh->vboC);
	glBindBuffer(GL_ARRAY_BUFFER, sceneMesh->vboC);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * sceneColors.size(), sceneColors.data(), GL_STATIC_DRAW);
	
	int colorLoc = sceneShader->attributeLocation("color_in");
	sceneShader->enableAttributeArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)(0));//(void *)(3 * sizeof(Triangle)));//(void *)(3 * sizeof(QVector3D)));
	
	glGenBuffers(1, &sceneMesh->vboN);
	glBindBuffer(GL_ARRAY_BUFFER, sceneMesh->vboN);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D) * normals.size(), normals.data(), GL_STATIC_DRAW);

	int normalLoc = sceneShader->attributeLocation("normal");
	sceneShader->enableAttributeArray(normalLoc);
	glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));//(void *)(3 * sizeof(Triangle)));//(void *)(3 * sizeof(QVector3D)));

	assert(glGetError() == GL_NO_ERROR);
	/*
	glGenBuffers(1, &sceneMesh->eao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneMesh->eao);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sceneMesh->indicesNr * sizeof(unsigned int), &scene->mTriangleIdx.data()[0], GL_STATIC_DRAW);
	*/
}

void SceneDrawer::setBBoxBuffers()
{
	GLfloat vertices[] = {	
		
		-0.5, -0.5, +0.5,
		+0.5, -0.5, +0.5,
		+0.5, +0.5, +0.5,
		-0.5, +0.5, +0.5,
		-0.5, -0.5, -0.5,
		+0.5, -0.5, -0.5,
		+0.5, +0.5, -0.5,
		-0.5, +0.5, -0.5,
	};
	GLfloat colors[] = {
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
	};
	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 7,
		5, 7, 6,
		4, 0, 3,
		4, 3, 7,
		1, 5, 4,
		1, 4, 0,
		3, 2, 6,
		3, 6, 7,
	};
	bboxMesh->indicesNr = 12 * 3;

	glGenVertexArrays(1, &bboxMesh->vao);
	assert(glGetError() == GL_NO_ERROR);
	glBindVertexArray(bboxMesh->vao);
	assert(glGetError() == GL_NO_ERROR);

	glGenBuffers(1, &bboxMesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bboxMesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, vertices, GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int vertexLoc = bboxShader->attributeLocation("position");
	bboxShader->enableAttributeArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	glGenBuffers(1, &bboxMesh->vboC);
	glBindBuffer(GL_ARRAY_BUFFER, bboxMesh->vboC);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8 * 3, colors, GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

	int colorLoc = bboxShader->attributeLocation("color_in");
	bboxShader->enableAttributeArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0));
	assert(glGetError() == GL_NO_ERROR);

	glGenBuffers(1, &bboxMesh->eao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bboxMesh->eao);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * bboxMesh->indicesNr, indices, GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);

}

void SceneDrawer::setBBoxVertices(BVH *bvh, BVHNode *node)
{
	unsetBBoxMesh();
	showBBox = true;
	GLfloat vertices[] = {
		node->bounds[0][0], node->bounds[0][1], node->bounds[1][2], // 0
		node->bounds[1][0], node->bounds[0][1], node->bounds[1][2],	// 1
		node->bounds[1][0], node->bounds[1][1], node->bounds[1][2],	// 2
		node->bounds[0][0], node->bounds[1][1], node->bounds[1][2],	// 3
		node->bounds[0][0], node->bounds[0][1], node->bounds[0][2],	// 4
		node->bounds[1][0], node->bounds[0][1], node->bounds[0][2],	// 5
		node->bounds[1][0], node->bounds[1][1], node->bounds[0][2],	// 6 
		node->bounds[0][0], node->bounds[1][1], node->bounds[0][2]	// 7
	};

	glBindVertexArray(bboxMesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, bboxMesh->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 3 * sizeof(GLfloat), vertices);

	
	std::vector<unsigned> triangles = getPrimitiveIndices(bvh, node);
	
	Color c;
	c.color[0] = (char)0;
	c.color[1] = (char)255;
	c.color[2] = (char)0;
	c.color[3] = (char)0;

	std::vector<Color> localColors;
	qDebug() << sizeof(localColors);
	localColors.push_back(c);
	localColors.push_back(c);
	localColors.push_back(c);

	qDebug() << sizeof(localColors.data());
	qDebug() << sizeof(Color);

	glBindVertexArray(sceneMesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, sceneMesh->vboC);
	for (std::vector<unsigned>::iterator it = triangles.begin(); it != triangles.end(); it++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, scene->mTriangleIdx[currentBVHIndex][*it] * 3 * 4, sizeof(Color) * 3, localColors.data());
	}
	
}

void SceneDrawer::unsetBBoxMesh()
{
	showBBox = false;

	glBindVertexArray(sceneMesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, sceneMesh->vboC);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Color) * sceneColors.size(), sceneColors.data());
}

std::vector<unsigned> SceneDrawer::getPrimitiveIndices(BVH *bvh, BVHNode *n) const
{
	std::vector<unsigned> result;
	if (n->axis == 3)
	{
		for (unsigned i = 0; i < n->children; i++)
			result.push_back(n->child + i);
	}
	else if (n->axis < 3)
	{
		std::vector<unsigned> leftTriangles = getPrimitiveIndices(bvh, &bvh->mNodes[n->child]);
		std::vector<unsigned> rightTriangles = getPrimitiveIndices(bvh, &bvh->mNodes[n->child + 1]);

		result.insert(result.end(),  leftTriangles.begin(), leftTriangles.end());
		result.insert(result.end(), rightTriangles.begin(), rightTriangles.end());
	}
	return result;
}


void SceneDrawer::draw(QMatrix4x4 *projection, QMatrix4x4 *view, QMatrix4x4 *model, PointLight *light)
{
	sceneShader->bind();
	assert(glGetError() == GL_NO_ERROR);

	sceneShader->setUniformValue("mvp_matrix", *projection * *view * *model);
	assert(glGetError() == GL_NO_ERROR);

	sceneShader->setUniformValue("model", *model);
	assert(glGetError() == GL_NO_ERROR);

	QVector4D lightposition = (*model) * QVector4D(light->position, 1.0);
	sceneShader->setUniformValue("light.position", QVector3D(lightposition));
	assert(glGetError() == GL_NO_ERROR);

	sceneShader->setUniformValue("light.diffuse", light->diffuse);
	assert(glGetError() == GL_NO_ERROR);
	sceneMesh->drawArrays();

	if (showBBox) 
	{
		bboxShader->bind();
		assert(glGetError() == GL_NO_ERROR);
		bboxShader->setUniformValue("mvp_matrix", *projection * *view * *model);
		assert(glGetError() == GL_NO_ERROR);
		bboxMesh->drawElements();
	}
}
