#include "VAO.h"

AIR_NAMESPACE_BEGIN

VAO::VAO() {
	glGenVertexArrays(1, &m_vao);
	AIR_LOG("VAO [id=%d] inited", m_vao);
}


void VAO::bind() {
	glBindVertexArray(m_vao);
}


void VAO::unbind() {
	glBindVertexArray(0);
}


/*
	size - dimentions (2d = 2, 3d = 3, ...)
*/
VBO& VAO::pushVBO(GLuint vboIndex, GLuint vaoIndex, GLuint size, GLsizei stride, GLsizei offset) {
	this->bind();
	VBO& tmp = m_vbo[vboIndex];
	tmp.bind();
	glVertexAttribPointer(vaoIndex, size, GL_FLOAT, GL_FALSE, stride, (const void*)offset);
	glEnableVertexAttribArray(vaoIndex);
	this->unbind();
	return tmp;
}

VBO& VAO::get_VBO(GLuint vboIndex) {
	return m_vbo[vboIndex];
}

VAO::~VAO() {
	AIR_LOG("VAO [id=%d] destructed", m_vao);
	glDeleteVertexArrays(1, &m_vao);
}

AIR_NAMESPACE_END