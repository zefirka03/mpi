#pragma once
#include "core/AirCore.h"
#include "GridTexture.h"

#include <algorithm>

//https://www.google.com/search?q=%D0%9B%D0%B0%D0%BA%D1%81%D0%B0-%D0%92%D0%B5%D0%BD%D0%B4%D1%80%D0%BE%D1%84%D1%84%D0%B0+c%5Btvf+python&sca_esv=a5b03d48c1dcd4f4&sca_upv=1&ei=KMIXZuPQCIu6wPAP1OqgkAY&ved=0ahUKEwjjhOSygLqFAxULHRAIHVQ1CGIQ4dUDCBA&uact=5&oq=%D0%9B%D0%B0%D0%BA%D1%81%D0%B0-%D0%92%D0%B5%D0%BD%D0%B4%D1%80%D0%BE%D1%84%D1%84%D0%B0+c%5Btvf+python&gs_lp=Egxnd3Mtd2l6LXNlcnAiKtCb0LDQutGB0LAt0JLQtdC90LTRgNC-0YTRhNCwIGNbdHZmIHB5dGhvbki7C1C9AVidCnABeACQAQCYAYEBoAGdBaoBAzYuMrgBA8gBAPgBAZgCBKAChQLCAgoQABhHGNYEGLADwgIEECEYFcICCRAhGAoYoAEYKpgDAOIDBRIBMSBAiAYBkAYDkgcBNKAHlgs&sclient=gws-wiz-serp#fpstate=ive&vld=cid:60cb31b5,vid:VEuNAbqPK6U,st:0
class Solution {
private:
    glm::vec2 h = glm::vec2(0.5);
    float t = 0;
public:

    struct Cell{
        Cell() { u = 0.0; }
        double u = 0.0;
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
                double color = std::max(0.0, std::min(255.0, 128 + curr_cells[x][y].u * 255.f * 0.5));
                texture.set_pixel(x, y, { 
                        uint8_t(color), 
                        uint8_t(color),
                        uint8_t(color) 
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

    void iterate(double deltaTime) {
        t += deltaTime;

        for (int y = 0; y < m_size_y; y++) {
            for (int x = 0; x < m_size_x; x++) {
                glm::vec2 a = m_speed(x, y);

#define u(_x,_y) get_cell(_x,_y).u

                back_cells[x][y].u = u(x, y) -
                    deltaTime * a.x * (u(x + 1, y) - u(x - 1, y)) / (2.0 * h.x) -
                    deltaTime * a.y * (u(x, y + 1) - u(x, y - 1)) / (2.0 * h.y) +
                    deltaTime * deltaTime * a.x * a.x * (u(x - 1, y) - 2.0 * u(x, y) + u(x + 1, y)) / (2.0 * h.x * h.x) +
                    deltaTime * deltaTime * a.y * a.y * (u(x, y - 1) - 2.0 * u(x, y) + u(x, y + 1)) / (2.0 * h.y * h.y) +
                    deltaTime * deltaTime * (a.x * a.y + a.y * a.x) * (u(x + 1, y + 1) - u(x - 1, y + 1) - u(x + 1, y - 1) + u(x - 1, y - 1)) / (8.0 * h.y * h.x);
                    
            }
        }
        swap_data();
        printf("%f\n", get_cell(256, 256).u);
    }

private:
    std::vector<std::vector<Cell>> curr_cells;
    std::vector<std::vector<Cell>> back_cells;

    glm::vec2(*m_speed)(double, double) = nullptr;

    int m_size_x;
    int m_size_y;
};