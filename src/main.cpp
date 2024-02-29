#include "core/AirCore.h"
#include "renderer/renderer.h"


class PrefabCamera2D : public air::Prefab<Transform2D, Camera2D> {
public:
    PrefabCamera2D() {};
};


struct scene : public air::Scene<SystemRenderer2D> {
    void start() override {
        PrefabCamera2D camera;
        camera.get_component<Camera2D>().resize(1280, 720);

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                air::Entity sprite;
                auto& transform = sprite.add_component<Transform2D>();
                auto& circle = sprite.add_component<SpriteCircle>();

                circle.radius = 50;
                transform.position = glm::vec2(i * 100, j * 100);
            }
        }

    };
};


int main() {
    air::Game game(1280, 720, "App");
    game.run(new scene());

    return 0;
}