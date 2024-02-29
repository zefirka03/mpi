#include "renderer.h"


Camera2D::Camera2D(float width, float height) {
	resize(width, height);
}


Camera2D::~Camera2D() {}


void Camera2D::resize(float width, float height) {
	m_projection = glm::ortho(0.f, width, height, 0.f);
}


glm::mat4 Camera2D::get_projection() {
	glm::mat4 tmp(1);
	Transform2D& transform = get_entity().get_component<Transform2D>();
	tmp = glm::scale(tmp, glm::vec3(transform.scale, 1));
	tmp = glm::translate(tmp, glm::vec3(transform.position, 0));
	return tmp * m_projection;
}


Renderer2D::Renderer2D() {
	m_shader.load_from_string(
	std::string({ R"(~~vertex~~
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 color;

uniform mat4 proj;

out vec3 o_color;
out vec2 o_texCoords;

void main() {
	gl_Position = proj * vec4(position, 0.0, 1.0); 
	o_color = color;
	o_texCoords = texCoords;
}
 
~~fragment~~
#version 430 core

out vec4 out_color;

in vec3 o_color;
in vec2 o_texCoords;

void main() {
	if(length(vec2(0.5) - o_texCoords) <= 0.5)
		out_color = vec4(o_color, 1.0);
	else out_color = vec4(1, 1, 1, 0);
}
)" }).c_str(),
	AIR_SHADER_VF);

	m_vao.pushVBO(0, 0, 2, sizeof(DeviceVertexInstance), 0 * sizeof(GLfloat)).buffer(nullptr, 0);
	m_vao.pushVBO(0, 1, 2, sizeof(DeviceVertexInstance), 2 * sizeof(GLfloat));
	m_vao.pushVBO(0, 2, 3, sizeof(DeviceVertexInstance), 4 * sizeof(GLfloat));
}


void Renderer2D::drawCircle(HostCircleInstance&& sprite) {
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(-sprite.radius, -sprite.radius), glm::vec2(0, 0), sprite.color }));
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(-sprite.radius, +sprite.radius), glm::vec2(0, 1), sprite.color }));
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(+sprite.radius, +sprite.radius), glm::vec2(1, 1), sprite.color }));
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(-sprite.radius, -sprite.radius), glm::vec2(0, 0), sprite.color }));
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(+sprite.radius, +sprite.radius), glm::vec2(1, 1), sprite.color }));
	m_data.emplace_back(DeviceVertexInstance({ sprite.position + glm::vec2(+sprite.radius, -sprite.radius), glm::vec2(1, 0), sprite.color }));
}


void Renderer2D::submit(Camera2D& camera) {
	m_shader.set_matrix4f(camera.get_projection(), "proj");

	m_vao.get_VBO(0).buffer(m_data.data(), m_data.size() * sizeof(DeviceVertexInstance));
	m_vao.bind();
	m_shader.use();
	glDrawArrays(GL_TRIANGLES, 0, m_data.size());
	m_shader.unuse();
	m_vao.unbind();

	m_data.clear();
}


void SystemRenderer2D::update() {
	auto& reg = air::ECS::registry;

	Camera2D* active_camera = nullptr;
	reg.view<Camera2D>().each([&](Camera2D& camera) {
		active_camera = &camera;
		});
	if (!active_camera) return;

	reg.view<SpriteCircle>().each([&](SpriteCircle& sprite) {
		auto transform_pos = sprite.get_entity().get_component<Transform2D>().position;
		auto radius = sprite.radius;
		auto color = sprite.color;

		m_renderer->drawCircle({ radius, {transform_pos.x, transform_pos.y}, color });
		});

	m_renderer->submit(*active_camera);
}