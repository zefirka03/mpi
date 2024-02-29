#include <vector>

#include "../core/AirCore.h"

struct Transform2D : public air::Component {
    glm::vec2 position = glm::vec2(0);
	glm::vec2 scale = glm::vec2(1);
};


struct SpriteCircle : public air::Component {
    float radius = 10.0f;
    glm::vec3 color = glm::vec3(1);
};


class Camera2D : public air::Component {
public:
	Camera2D() {};
	Camera2D(float width, float height);
	~Camera2D();

	void resize(float width, float height);
	glm::mat4 get_projection();

private:
	glm::mat4 m_projection;
};


class Renderer2D {
public:
	struct DeviceVertexInstance {
		glm::vec2 position;
		glm::vec2 texCoords;
		glm::vec3 color;
	};

	struct HostCircleInstance {
		float radius;
		glm::vec2 position;
		glm::vec3 color;
	};

	Renderer2D();
	~Renderer2D() {}
	
	void drawCircle(HostCircleInstance&& sprite);
	void submit(Camera2D& camera);

private:
	friend class SystemRenderer2D;
	std::vector<DeviceVertexInstance> m_data;
	air::Shader m_shader;
	air::VAO m_vao;
};



class SystemRenderer2D : public air::System {
public:
	SystemRenderer2D(){ m_renderer = new Renderer2D(); }
	~SystemRenderer2D() { delete m_renderer; }

    void start() override {
		AIR_LOG("Renderer2D start");
    }

	void update() override;
private:
	Renderer2D* m_renderer;
};