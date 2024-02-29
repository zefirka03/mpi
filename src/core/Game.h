#pragma once
#include "ogl_common.h"
#include "AirUtils.h"
#include "VAO.h"
#include "Scene.h"

#include <unordered_map>

AIR_NAMESPACE_BEGIN

class Game {
public:
	Game(int width, int height, const char* title);
	~Game();

	void run(SceneBase* scene);
	static Game* get_current_game();
	void go_to_scene(SceneBase* scene);
	GLFWwindow* get_glfw_window();
private:
	static Game* m_handler;
	GLFWwindow* m_window = nullptr;
	struct windowParameters {
		int width;
		int height;
		const char* title;
	} m_parameters;
	SceneBase* m_current_scene = nullptr;
};

AIR_NAMESPACE_END