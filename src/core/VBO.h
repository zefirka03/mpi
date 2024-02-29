#pragma once
#include "ogl_common.h"
#include "AirUtils.h"

AIR_NAMESPACE_BEGIN

class VBO {
public:
	VBO();
	~VBO();

	void bind();
	void unbind();
	void buffer(void* data, size_t size);
	void rebuffer(void* data, size_t offset, size_t size);
	operator GLuint();
private:
	GLuint m_vbo;
};

AIR_NAMESPACE_END