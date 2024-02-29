#include "VBO.h"

AIR_NAMESPACE_BEGIN

VBO::VBO() {
    glGenBuffers(1, &m_vbo);
    AIR_LOG("VBO [id=%d] inited", m_vbo);
}


void VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}


void VBO::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/*
    size in bytes
*/
void VBO::buffer(void* data, size_t size) {
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
    this->unbind();
}


void VBO::rebuffer(void* data, size_t offset, size_t size) {
    this->bind();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    this->unbind();
}


VBO::operator GLuint() {
    return m_vbo;
}


VBO::~VBO() {
    glDeleteBuffers(1, &m_vbo);
    AIR_LOG("VBO [id=%d] destructed", m_vbo);
}

AIR_NAMESPACE_END