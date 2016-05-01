#pragma once

#include "colorMapping.h"

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>
#include <assert.h>


class Mesh : protected QOpenGLFunctions_4_3_Core
{
public:
	Mesh(QOpenGLShaderProgram *program);
	~Mesh();
	void drawArrays();
	void drawArraysPoints();
	void drawElements();
	void setTriangles();

	GLuint vao, vbo, vboC, vboN, eao, vboW, vboH;
	unsigned indicesNr;
	QOpenGLShaderProgram *shaderProgram;
};