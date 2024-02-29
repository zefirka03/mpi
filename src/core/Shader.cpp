#include "Shader.h"

namespace air {

	Shader::Shader() {
		this->m_inited = false;
	}

	Shader::Shader(const char* path, uint8_t usings) {
		this->load_from_file(path, usings);
	}

	void Shader::use() {
#ifdef AIR_DEBUG
		if (!m_inited) { AIR_LOG("Shader does not inited"); return; }
#endif
		glUseProgram(this->m_progId);
	}

	void Shader::unuse() {
		glUseProgram(0);
	}

	bool Shader::is_inited() {
		return m_inited;
	}
	GLuint Shader::get_id() {
		return this->m_progId;
	}

	void Shader::load_from_file(const char* path, uint8_t usings) {
		this->m_progId = glCreateProgram();
		std::ifstream of;
		of.open(path);

		if (!of.good()) AIR_LOG("Shader file is not loaded!");

		std::string curr;
		int type = 0;
		std::string vertex_source = "",
			geometry_source = "",
			fragment_source = "";
		while (std::getline(of, curr)) {
			if (curr == "~~vertex~~") type = 0;
			else if (curr == "~~geometry~~") type = 1;
			else if (curr == "~~fragment~~") type = 2;
			else {
				if (type == 0) vertex_source += curr + "\n";
				else if (type == 1) geometry_source += curr + "\n";
				else if (type == 2) fragment_source += curr + "\n";
			}
		}

		const char* vertex_source_c = vertex_source.c_str(),
			* fragment_source_c = fragment_source.c_str(),
			* geometry_source_c = geometry_source.c_str();

		GLuint	vertex_id = glCreateShader(GL_VERTEX_SHADER),
			geometry_id = glCreateShader(GL_GEOMETRY_SHADER),
			fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertex_id, 1, &vertex_source_c, 0);
		glShaderSource(geometry_id, 1, &geometry_source_c, 0);
		glShaderSource(fragment_id, 1, &fragment_source_c, 0);

		glCompileShader(vertex_id);
		glCompileShader(fragment_id);
		if (usings == AIR_SHADER_VGF)
			glCompileShader(geometry_id);

#ifdef AIR_DEBUG
		AIR_LOG("Shader::Debugging");
		GLchar info[2048];
		glGetShaderInfoLog(vertex_id, 2048, 0, info);
		AIR_LOG(info);
		glGetShaderInfoLog(geometry_id, 2048, 0, info);
		AIR_LOG(info);
		glGetShaderInfoLog(fragment_id, 2048, 0, info);
		AIR_LOG(info);
#endif

		glAttachShader(this->m_progId, vertex_id);
		if (usings == AIR_SHADER_VGF)
			glAttachShader(this->m_progId, geometry_id);
		glAttachShader(this->m_progId, fragment_id);

		glLinkProgram(this->m_progId);

		glDeleteShader(vertex_id);
		glDeleteShader(geometry_id);
		glDeleteShader(fragment_id);

		this->m_inited = true;
	}

	void Shader::load_from_string(const char* string, uint8_t usings) {
		this->m_progId = glCreateProgram();
		std::string curr;
		int type = 0;
		std::string vertex_source = "",
			geometry_source = "",
			fragment_source = "";
		std::istringstream sa(string);
		while (std::getline(sa, curr)) {
			if (curr == "~~vertex~~") type = 0;
			else if (curr == "~~geometry~~") type = 1;
			else if (curr == "~~fragment~~") type = 2;
			else {
				if (type == 0) vertex_source += curr + "\n";
				else if (type == 1) geometry_source += curr + "\n";
				else if (type == 2) fragment_source += curr + "\n";
			}
		}

		const char* vertex_source_c = vertex_source.c_str(),
			* fragment_source_c = fragment_source.c_str(),
			* geometry_source_c = geometry_source.c_str();

		GLuint	vertex_id = glCreateShader(GL_VERTEX_SHADER),
			geometry_id = glCreateShader(GL_GEOMETRY_SHADER),
			fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vertex_id, 1, &vertex_source_c, 0);
		glShaderSource(geometry_id, 1, &geometry_source_c, 0);
		glShaderSource(fragment_id, 1, &fragment_source_c, 0);

		glCompileShader(vertex_id);
		glCompileShader(fragment_id);
		if (usings == AIR_SHADER_VGF)
			glCompileShader(geometry_id);

#ifdef AIR_DEBUG
		GLchar info[2048];
		glGetShaderInfoLog(vertex_id, 2048, 0, info);
		AIR_LOG(info);
		glGetShaderInfoLog(geometry_id, 2048, 0, info);
		AIR_LOG(info);
		glGetShaderInfoLog(fragment_id, 2048, 0, info);
		AIR_LOG(info);
#endif

		glAttachShader(this->m_progId, vertex_id);
		if (usings == AIR_SHADER_VGF)
			glAttachShader(this->m_progId, geometry_id);
		glAttachShader(this->m_progId, fragment_id);

		glLinkProgram(this->m_progId);

		glDeleteShader(vertex_id);
		glDeleteShader(geometry_id);
		glDeleteShader(fragment_id);

		this->m_inited = true;
	}

	GLuint Shader::_request_location(const char* path) {
		GLuint loc;
		if (m_locationsCache.find(path) != m_locationsCache.end())
			loc = m_locationsCache[path];
		else loc = m_locationsCache.insert(std::make_pair<std::string_view, GLuint>(path, glGetUniformLocation(this->m_progId, path))).first->second;
		return loc;
	}

	void Shader::set_matrix4f(glm::mat4 val, const char* path) {
		this->use();
		glUniformMatrix4fv(_request_location(path), 1, GL_FALSE, glm::value_ptr(val));
		this->unuse();
	}

	void Shader::set_float(GLfloat val, const char* path) {
		this->use();
		glUniform1f(_request_location(path), val);
		this->unuse();
	}

	void Shader::set_vector2f(glm::vec2 val, const char* path) {
		this->use();
		glUniform2f(_request_location(path), val.x, val.y);
		this->unuse();
	}

	void Shader::set_vector4f(glm::vec4 val, const char* path) {
		this->use();
		glUniform4f(_request_location(path), val.r, val.g, val.b, val.a);
		this->unuse();
	}

	Shader::~Shader() {
		glDeleteProgram(m_progId);
	}

}