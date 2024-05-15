#pragma once
#include "core/AirCore.h"
#include "GridTexture.h"

class Solution {
private:
    glm::vec2 h = glm::vec2(1, 1);
    float t = 0;
public:

    struct Cell{
        Cell() { u = 0.0f; }
        float u = 0.f;
    };

    Solution(
        int size_x, 
        int size_y, 
        double (*u0_function)(double, double),
        glm::vec2 (*speed)(double, double)
    ){
        m_size_x = size_x;
        m_size_y = size_y;

        m_speed = speed;

        curr_cells = std::vector<std::vector<Cell>>(size_x, std::vector<Cell>(size_y, Cell()));
        back_cells = std::vector<std::vector<Cell>>(size_x, std::vector<Cell>(size_y, Cell()));

        for(int y=0; y<size_y; y++){
            for(int x=0; x<size_x; x++){
                curr_cells[x][y].u = u0_function(x, y);
            }
        }
    }

    void apply_to_texture(GridTexture& texture){
        texture.resize(m_size_x, m_size_y);

        for(int y=0; y<m_size_y; y++){
            for(int x=0; x<m_size_x; x++){
                texture.set_pixel(x, y, { 
                        uint8_t(curr_cells[x][y].u * 255.f), 
                        uint8_t(curr_cells[x][y].u * 255.f),
                        uint8_t(curr_cells[x][y].u * 255.f) 
                    }
                );
            }
        }

        texture.update();
    }

    Cell get_cell(int x, int y) const {
        if (x < 0 || x >= m_size_x || y < 0 || y >= m_size_y)
            return Cell();
        return curr_cells[x][y];
    }

    void swap_data() {
        curr_cells.swap(back_cells);
    }

    void iterate(float deltaTime) {
        t += deltaTime;
        for (int y = 0; y < m_size_y; y++) {
            for (int x = 0; x < m_size_x; x++) {
                glm::vec2 a = m_speed(x, y);

                float x_comp = a.x >= 0.f ? get_cell(x, y).u - get_cell(x - 1, y).u : get_cell(x + 1, y).u - get_cell(x, y).u;
                float y_comp = a.y >= 0.f ? get_cell(x, y).u - get_cell(x, y - 1).u : get_cell(x, y + 1).u - get_cell(x, y).u;

                back_cells[x][y].u = get_cell(x, y).u - deltaTime * ((a.x * x_comp / h.x) + (a.y * y_comp / h.y));
            }
        }
        swap_data();
    }

private:
    std::vector<std::vector<Cell>> curr_cells;
    std::vector<std::vector<Cell>> back_cells;

    glm::vec2(*m_speed)(double, double) = nullptr;

    int m_size_x;
    int m_size_y;
};