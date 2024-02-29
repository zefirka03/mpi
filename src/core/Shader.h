#pragma once
#include "AirUtils.h"
#include "ogl_common.h"

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <string>

#define AIR_DEBUG

#define AIR_SHADER_VF 0
#define AIR_SHADER_VGF 1

namespace air {

    class Shader {
    public:
        Shader();
        // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
        Shader(const char* path, uint8_t usings);
        void use();
        void unuse();
        // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
        void load_from_file(const char* path, uint8_t usings);
        // usings - AIR_SHADER_VF or AIR_SHADER_VGF (use VGF when you using geometry shader)
        void load_from_string(const char* string, uint8_t usings);
        bool is_inited();
        GLuint get_id();


        void set_matrix4f(glm::mat4 val, const char* path);
        void set_vector2f(glm::vec2 val, const char* path);
        void set_vector4f(glm::vec4 val, const char* path);
        void set_float(GLfloat val, const char* path);

        ~Shader();
    private:
        std::unordered_map<std::string_view, GLuint> m_locationsCache;

        GLuint m_progId = 0;
        bool m_inited;

        GLuint _request_location(const char* path);
    };

}