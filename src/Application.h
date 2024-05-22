#pragma once
#include "core/AirCore.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <mpi.h>

#include "GridTexture.h"
#include "Solution2.h"


constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;


struct DeviceVertexInstance {
    glm::vec2 position;
    glm::vec2 texCoord;
};


class Lab2_Solver : public Solver {
public:
    double solve(int cell_index, std::vector<Cell> const& cells, glm::vec2 a, double deltaTime) override {
        double bound_u = 0.f;

        auto& cell = cells[cell_index];

        int c_up_i = cell.faces[Face::FaceDirection::up].neib_index;
        int c_down_i = cell.faces[Face::FaceDirection::down].neib_index;
        int c_left_i = cell.faces[Face::FaceDirection::left].neib_index;
        int c_right_i = cell.faces[Face::FaceDirection::right].neib_index;

        double c_up_u = c_up_i != CELL_NULL ? cells[c_up_i].u : bound_u;
        double c_down_u = c_down_i != CELL_NULL ? cells[c_down_i].u : bound_u;
        double c_left_u = c_left_i != CELL_NULL ? cells[c_left_i].u : bound_u;
        double c_right_u = c_right_i != CELL_NULL ? cells[c_right_i].u : bound_u;

        int c_ul_i = c_up_i != CELL_NULL ? cells[c_up_i].faces[Face::FaceDirection::left].neib_index : CELL_NULL;
        int c_ur_i = c_up_i != CELL_NULL ? cells[c_up_i].faces[Face::FaceDirection::right].neib_index : CELL_NULL;
        int c_dl_i = c_down_i != CELL_NULL ? cells[c_down_i].faces[Face::FaceDirection::left].neib_index : CELL_NULL;
        int c_dr_i = c_down_i != CELL_NULL ? cells[c_down_i].faces[Face::FaceDirection::right].neib_index : CELL_NULL;

        double c_ul_u = c_ul_i != CELL_NULL ? cells[c_ul_i].u : bound_u;
        double c_ur_u = c_ur_i != CELL_NULL ? cells[c_ur_i].u : bound_u;
        double c_dl_u = c_dl_i != CELL_NULL ? cells[c_dl_i].u : bound_u;
        double c_dr_u = c_dr_i != CELL_NULL ? cells[c_dr_i].u : bound_u;

        return cell.u -
            deltaTime * a.x * (c_right_u - c_left_u) / (2.0 * cell.size.x) -
            deltaTime * a.y * (c_up_u - c_down_u) / (2.0 * cell.size.y) +
            deltaTime * deltaTime * a.x * a.x * (c_left_u - 2.0 * cell.u + c_right_u) / (2.0 * cell.size.x * cell.size.x) +
            deltaTime * deltaTime * a.y * a.y * (c_down_u - 2.0 * cell.u + c_up_u) / (2.0 * cell.size.y * cell.size.y) +
            deltaTime * deltaTime * (a.x * a.y + a.y * a.x) * (c_ur_u - c_ul_u - c_dr_u + c_dl_u) / (8.0 * cell.size.y * cell.size.x);

    }
};

/*
class Lab3_Solver : public Solver {
public:
    double solve(int cell_index, std::vector<Cell> const& cells, glm::vec2 a, double deltaTime) override {
        Lab1_Solver solve_up;
        Lab2_Solver solve_lax;

        double F_up = solve_up.solve(cell_index, cells, a, deltaTime);
        double F_lax = solve_lax.solve(cell_index, cells, a, deltaTime);

        auto phi = [](double r) -> bool {

        };

        return F_up + phi(cells[cell_index] - ) * (F_lax - F_up);
    }
};
*/


class Application {
public:
    static int run_child(int rank) {
        int32_t buffer_size;
        MPI_Recv(&buffer_size, 1, MPI_INT32_T, 0, SLVR_MPI_TAG_LOCALS_SEND_DATA_COUNT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<std::byte> locals_buffer(buffer_size);
        MPI_Recv(locals_buffer.data(), buffer_size, MPI_BYTE, 0, SLVR_MPI_TAG_LOCALS_SEND_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Mesh mesh;
        mesh.unpack_locals(locals_buffer);

        while(true){
            mesh.iterate(0.1f);

            std::vector<std::byte> packed = mesh.pack_locals();
            int32_t buffer_size = packed.size();
            MPI_Send(&buffer_size, 1, MPI_INT32_T, 0, SLVR_MPI_TAG_LOCALS_ITERATION_COUNT, MPI_COMM_WORLD);
            MPI_Send(packed.data(), buffer_size, MPI_BYTE, 0, SLVR_MPI_TAG_LOCALS_ITERATION, MPI_COMM_WORLD);
        }

        return 0;
    }

    static int run_parent() {
        if (!glfwInit())
            return 1;

        GLFWwindow* m_window = glfwCreateWindow(WIDTH, HEIGHT, "lab", NULL, NULL);
        if (!m_window) {
            glfwTerminate();
            return 1;
        }

        glfwMakeContextCurrent(m_window);

        glewInit();
        glfwSwapInterval(1);

        air::Shader shader;
        shader.load_from_string(
            std::string({ R"(~~vertex~~
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 o_texCoord;

void main() {
	gl_Position = vec4(position, 0.0, 1.0); 
    o_texCoord = texCoord;
}
 
~~fragment~~
#version 430 core

out vec4 out_color;
in vec2 o_texCoord;

uniform sampler2D uTexture;

void main() {
	out_color = texture(uTexture, o_texCoord);
}
)" }).c_str(),
AIR_SHADER_VF);

        std::vector<DeviceVertexInstance> vert_data = {
            DeviceVertexInstance{glm::vec2(-1.0, -1.0), glm::vec2(0.0, 0.0)},
            DeviceVertexInstance{glm::vec2(+1.0, -1.0), glm::vec2(1.0, 0.0)},
            DeviceVertexInstance{glm::vec2(+1.0, +1.0), glm::vec2(1.0, 1.0)},
            DeviceVertexInstance{glm::vec2(-1.0, -1.0), glm::vec2(0.0, 0.0)},
            DeviceVertexInstance{glm::vec2(+1.0, +1.0), glm::vec2(1.0, 1.0)},
            DeviceVertexInstance{glm::vec2(-1.0, +1.0), glm::vec2(0.0, 1.0)},
        };

        air::VAO vao;
        vao.pushVBO(0, 0, 2, sizeof(DeviceVertexInstance), 0 * sizeof(GLfloat)).buffer(vert_data.data(), sizeof(DeviceVertexInstance) * vert_data.size());
        vao.pushVBO(0, 1, 2, sizeof(DeviceVertexInstance), 2 * sizeof(GLfloat));

        int sz = 400;
        GridTexture grid(sz, sz);

        Solution2 sol(400, 400, 400, 400,
            new Lab2_Solver(),
            [](double x, double y) -> double {
                float sz = 400 * 0.5;
                return
                    std::abs(x - sz) < 50 &&
                    std::abs(y - sz) < 50 ? 1 : 0.;
            },
            [](double x, double y) {
                //float sz = 512 * 0.5;
                //if (x != sz || y != sz)
                //    return glm::normalize(glm::vec2(x - sz, y - sz));
                //return glm::vec2(x / sz, y / sz);
                return glm::vec2(1);
            });

        Mesh mesh(400, 400, 400, 400,
            [](double x, double y) -> double {
                float sz = 400 * 0.5;
                return (x-sz)*(x-sz)+(y-sz)*(y-sz)<100*100 &&(x-sz)*(x-sz)+(y-sz)*(y-sz)>80*80?1:0;
            }
        );
        mesh.redistribute();
        mesh.apply_to_texture(grid);

        glClearColor(0.2, 0.2, 0.2, 0.2);
        while (!glfwWindowShouldClose(m_window)) {
            glClear(GL_COLOR_BUFFER_BIT);
            if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                mesh.collect();
                mesh.apply_to_texture(grid);
            }

            grid.bind_texture();
            shader.use();
            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vert_data.size());
            vao.unbind();

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }

        return 0;
	}
};