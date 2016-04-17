#include "Mesh.h"

Mesh::Mesh(QOpenGLShaderProgram *shaderProgram)
{
	initializeOpenGLFunctions();

	this->shaderProgram = shaderProgram;
	this->shaderProgram->bind();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vboC);
	glDeleteBuffers(1, &vboN);
	glDeleteBuffers(1, &eao);
	vao = vbo = vboC = vboN = eao = 0;
}

void Mesh::drawElements()
{
	glBindVertexArray(vao);
	assert(glGetError() == GL_NO_ERROR);
	glDrawElements(GL_TRIANGLES, indicesNr, GL_UNSIGNED_INT, NULL);
	assert(glGetError() == GL_NO_ERROR);
}

void Mesh::drawArrays()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, indicesNr * 3);
}
