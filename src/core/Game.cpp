#include "Game.h"
#include <vector>

AIR_NAMESPACE_BEGIN

Game* Game::m_handler = nullptr;

Game::Game(int width, int height, const char* title) 
	: m_parameters({ width, height, title }) {
    if (!glfwInit())
        return;

	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    glewInit();

    m_handler = this;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glfwWindowHint(GLFW_SAMPLES, 4);
}


void Game::run(SceneBase* scene) {
    go_to_scene(scene);

    glClearColor(0.2, 0.2, 0.2, 0.2);
    while (!glfwWindowShouldClose(m_window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        scene->_update_systems();
        scene->_last_update_systems();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}


Game* Game::get_current_game() {
    return m_handler;
}


GLFWwindow* Game::get_glfw_window() {
    return m_window;
}


void Game::go_to_scene(SceneBase* scene) {
    delete m_current_scene;
    m_current_scene = scene;
    scene->_start_systems();

    scene->start();
}


Game::~Game() {
    glfwDestroyWindow(m_window);
    delete m_current_scene;
    AIR_LOG("GAME DESTRUCTED");
}

AIR_NAMESPACE_END