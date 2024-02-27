#include <iostream>
#include "gl_includes.h"

int main() {
    glfwInit();

    GLFWwindow* win = glfwCreateWindow(1280, 720, "f", NULL, NULL);
    glfwMakeContextCurrent(win);

    gladLoadGL(glfwGetProcAddress);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        glfwSwapBuffers(win);
    }
    return 0;
}