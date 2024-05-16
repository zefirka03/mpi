#pragma once
#include <array>
#include <algorithm>
#include <unordered_map>
#include <mpi.h>

#include "core/AirCore.h"
#include "GridTexture.h"

#define CELL_NULL -1

#define SLVR_MPI_TAG_LOCALS_SEND_DATA_COUNT 1
#define SLVR_MPI_TAG_LOCALS_SEND_DATA 2


struct Face {
    enum FaceDirection {
        left = 0,
        up = 1,
        right = 2,
        down = 3
    };

    // индекс соседа в его массиве locals
    // (то есть если сосед удаленный, то index показывает
    // номер сосдней ячейки в массиве locals на том процессе)
    int neib_index = CELL_NULL;
    // индекс соседа в массиве aliens (если сосед удаленный)
    int alien = CELL_NULL;
    // ранг соседа
    int rank;
};


struct Cell {
    // номер в массиве locals
    int index = CELL_NULL;
    // ранг ячейки
    int rank; 

    double u = 0;
    double u_back = 0;

    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 center;

    std::array<Face, 4> faces;
};


class Solver {
public:
    virtual double solve(int cell_index, std::vector<Cell> const& cells, glm::vec2 a, double deltaTime) = 0;
};


class Mesh {
private:
    // Локальные ячейки
    std::vector<Cell> m_locals;
    // Ячейки с других процессов
    std::vector<Cell> m_aliens; 

    glm::vec2 m_grid_size;
    // Size of grid in world space

    int _get_new_rank(Cell const& cell){
        return 1 + int(cell.center.x / (m_grid_size.x / 2.f)) + 2 * int(cell.center.y / (m_grid_size.y / 2.f));
    }

public:
    Mesh() {}

    Mesh(
        double size_x,
        double size_y,
        int cells_x,
        int cells_y,
        double (*u0_function)(double, double)
    ) {
        initialize_grid(
            size_x,
            size_y,
            cells_x,
            cells_y,
            u0_function
        );
    }

    void initialize_grid(
        double size_x,
        double size_y,
        int cells_x,
        int cells_y,
        double (*u0_function)(double, double)
    ) {
        m_grid_size = glm::vec2(size_x, size_y);

        double cell_size_x = size_x / cells_x;
        double cell_size_y = size_y / cells_y;

        std::vector<std::vector<Cell>> temp_cells(cells_x, std::vector<Cell>(cells_y, Cell()));

        // Init cells base
        for (int y = 0; y < cells_y; ++y) {
            for (int x = 0; x < cells_x; ++x) {
                temp_cells[x][y].index = y * size_y + x;

                temp_cells[x][y].size = glm::vec2(cell_size_x, cell_size_y);
                temp_cells[x][y].position = glm::vec2(cell_size_x * x, cell_size_y * y);
                temp_cells[x][y].center = temp_cells[x][y].position + temp_cells[x][y].size / 2.f;

                temp_cells[x][y].u = u0_function(temp_cells[x][y].center.x, temp_cells[x][y].center.y);
            }
        }

        // Init cells faces
        for (int y = 0; y < cells_y; ++y) {
            for (int x = 0; x < cells_x; ++x) {
                int up = y + 1 < cells_y ? temp_cells[x][y + 1].index : CELL_NULL;
                int down = y - 1 >= 0 ? temp_cells[x][y - 1].index : CELL_NULL;
                int right = x + 1 < cells_x ? temp_cells[x + 1][y].index : CELL_NULL;
                int left = x - 1 >= 0 ? temp_cells[x - 1][y].index : CELL_NULL;

                temp_cells[x][y].faces[Face::FaceDirection::up].neib_index = up;
                temp_cells[x][y].faces[Face::FaceDirection::down].neib_index = down;
                temp_cells[x][y].faces[Face::FaceDirection::right].neib_index = right;
                temp_cells[x][y].faces[Face::FaceDirection::left].neib_index = left;
            }
        }

        // Copy
        m_locals = std::vector<Cell>(cells_x * cells_y, Cell());
        for (int i = 0; i < cells_y; ++i)
            std::copy(
                temp_cells[i].begin(),
                temp_cells[i].end(),
                m_locals.data() + cells_x * i
            );
    }

    void redistribute() {
        std::unordered_map<int, std::vector<Cell>> rank_locals;

        // just apply rank for every cell
        for(int i = 0; i < m_locals.size(); ++i){
            m_locals[i].rank = _get_new_rank(m_locals[i]);
            rank_locals[m_locals[i].rank].push_back(m_locals[i]);
        }

        // send locals data to every process
        for (auto rank_it : rank_locals) {
            int32_t buffer_size = rank_it.second.size() * sizeof(Cell);
            MPI_Send(&buffer_size, 1, MPI_INT32_T, rank_it.first, SLVR_MPI_TAG_LOCALS_SEND_DATA_COUNT, MPI_COMM_WORLD);
            MPI_Send(rank_it.second.data(), rank_it.second.size() * sizeof(Cell), MPI_BYTE, rank_it.first, SLVR_MPI_TAG_LOCALS_SEND_DATA, MPI_COMM_WORLD);
        }
    }

    void exchange() {
        for (int i = 0; i < m_locals.size(); ++i) {
            
        }
    }

    void apply_to_texture_rank_map(GridTexture& texture){
        for(int i = 0; i < m_locals.size(); ++i){
            glm::vec2 relate_pos = m_locals[i].center / m_grid_size;

            int x_p = int(relate_pos.x * texture.size().x);
            int y_p = int(relate_pos.y * texture.size().y);

            double color = std::max(0.0, std::min(255.0, (float)(m_locals[i].rank) * 255.0 * 0.25));
            texture.set_pixel(x_p, y_p, {
                    uint8_t(color),
                    uint8_t(color),
                    uint8_t(color)
                }
            );
        }
        
        texture.update();
    }

    std::vector<std::byte> pack_locals() {
        std::vector<std::byte> pocket(m_locals.size() * sizeof(Cell));
        std::memcpy(pocket.data(), m_locals.data(), m_locals.size() * sizeof(Cell));
        return pocket;
    }

    static std::vector<std::byte> pack_locals(std::vector<Cell> const& locals) {
        std::vector<std::byte> pocket(locals.size() * sizeof(Cell));
        std::memcpy(pocket.data(), locals.data(), locals.size() * sizeof(Cell));
        return pocket;
    }

    void unpack_locals(std::vector<std::byte> const& pocket_data) {
        m_locals.resize(pocket_data.size() * sizeof(std::byte) / sizeof(Cell));
        std::memcpy(m_locals.data(), pocket_data.data(), pocket_data.size() * sizeof(std::byte));
    }

};


class Solution2 {
private:
    std::vector<Cell> m_cells;

    glm::vec2 m_grid_size;

    Solver* m_solver_instance = nullptr;
    glm::vec2 (*m_speed_field)(double, double) = nullptr;

public:
    Solution2(
        double size_x,
        double size_y,
        int cells_x,
        int cells_y,
        Solver* solver,
        double (*u0_function)(double, double),
        glm::vec2(*speed_field)(double, double)
    ) {
        m_solver_instance = solver;
        m_speed_field = speed_field;
        m_grid_size = glm::vec2(size_x, size_y);

        double cell_size_x = size_x / cells_x;
        double cell_size_y = size_y / cells_y;

        std::vector<std::vector<Cell>> temp_cells(cells_x, std::vector<Cell>(cells_y, Cell()));

        // Init cells base
        for(int y=0; y<cells_y; ++y) {
            for(int x=0; x<cells_x; ++x) {
                temp_cells[x][y].index = y * size_y + x;

                temp_cells[x][y].size = glm::vec2(cell_size_x, cell_size_y);
                temp_cells[x][y].position = glm::vec2(cell_size_x * x, cell_size_y * y);
                temp_cells[x][y].center = temp_cells[x][y].position + temp_cells[x][y].size / 2.f;

                temp_cells[x][y].u = u0_function(temp_cells[x][y].center.x, temp_cells[x][y].center.y);
            }
        }

        // Init cells faces
        for(int y=0; y<cells_y; ++y){
            for(int x=0; x<cells_x; ++x){
                int up = y + 1 < cells_y ? temp_cells[x][y + 1].index : CELL_NULL;
                int down = y - 1 >= 0 ? temp_cells[x][y - 1].index : CELL_NULL;
                int right = x + 1 < cells_x ? temp_cells[x + 1][y].index : CELL_NULL;
                int left = x - 1 >= 0 ? temp_cells[x - 1][y].index : CELL_NULL;

                temp_cells[x][y].faces[Face::FaceDirection::up].neib_index = up;
                temp_cells[x][y].faces[Face::FaceDirection::down].neib_index = down;
                temp_cells[x][y].faces[Face::FaceDirection::right].neib_index = right;
                temp_cells[x][y].faces[Face::FaceDirection::left].neib_index = left;
            }
        }

        // Copy
        m_cells = std::vector<Cell>(cells_x * cells_y, Cell());
        for(int i=0; i< cells_y; ++i)
            std::copy(
                temp_cells[i].begin(),
                 temp_cells[i].end(), 
                 m_cells.data() + cells_x * i
            );
    }

    void iterate(float deltaTime) {
        for (int i = 0; i < m_cells.size(); ++i) {
            auto& cell = m_cells[i];
            cell.u_back = m_solver_instance->solve(i, m_cells, m_speed_field(cell.center.x, cell.center.y), deltaTime);
            std::swap(cell.u_back, cell.u);
        }
    }

    void apply_to_texture(GridTexture& texture){
        for(int i=0; i<m_cells.size(); ++i){
            glm::vec2 relate_pos = m_cells[i].center / m_grid_size;

            int x_p = int(relate_pos.x * texture.size().x);
            int y_p = int(relate_pos.y * texture.size().y);

            double color = std::max(0.0, std::min(255.0, 128 + m_cells[i].u * 255.f * 0.5));
            texture.set_pixel(x_p, y_p, {
                    uint8_t(color),
                    uint8_t(color),
                    uint8_t(color)
                }
            );
        }
        
        texture.update();
    }
};