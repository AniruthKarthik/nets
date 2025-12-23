#include "PuzzleGenerator.hpp"
#include "../src/cpp/Tile.hpp"
#include <vector>
#include <stack>
#include <random>
#include <algorithm>
#include <map>

void PuzzleGenerator::generate(Board& board) {
    int width = board.width;
    int height = board.height;

    // 1. Reset board and create a temporary grid to store connections
    board = Board(width, height, board.wraps);
    std::vector<std::vector<std::vector<Direction>>> temp_connections(height, std::vector<std::vector<Direction>>(width));

    // 2. Perform Randomized DFS to create a spanning tree
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::stack<std::pair<int, int>> s;
    std::random_device rd;
    std::mt19937 g(rd());

    int start_row = g() % height;
    int start_col = g() % width;
    s.push({start_row, start_col});
    visited[start_row][start_col] = true;

    while (!s.empty()) {
        auto [row, col] = s.top();

        std::vector<std::pair<std::pair<int, int>, Direction>> neighbors;
        
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, 1, -1};
        Direction dirs[] = {NORTH, SOUTH, EAST, WEST};

        for (int i = 0; i < 4; ++i) {
            int nr = row + dr[i];
            int nc = col + dc[i];
            if (nr >= 0 && nr < height && nc >= 0 && nc < width && !visited[nr][nc]) {
                neighbors.push_back({{nr, nc}, dirs[i]});
            }
        }

        if (neighbors.empty()) {
            s.pop();
        } else {
            std::shuffle(neighbors.begin(), neighbors.end(), g);
            auto [next_pos, dir] = neighbors[0];
            auto [next_row, next_col] = next_pos;
            
            s.push({next_row, next_col});
            visited[next_row][next_col] = true;

            temp_connections[row][col].push_back(dir);
            temp_connections[next_row][next_col].push_back(opposite(dir));
        }
    }

    // 3. Determine TileType and orientation from connections
    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            auto& conns = temp_connections[r][c];
            TileType type = EMPTY;
            int base_rotation = 0;

            std::sort(conns.begin(), conns.end());

            switch (conns.size()) {
                case 1:
                    type = PC;
                    if (conns[0] == EAST) base_rotation = 90;
                    else if (conns[0] == SOUTH) base_rotation = 180;
                    else if (conns[0] == WEST) base_rotation = 270;
                    break;
                case 2:
                    if (conns[0] == NORTH && conns[1] == SOUTH) { type = STRAIGHT; }
                    else if (conns[0] == EAST && conns[1] == WEST) { type = STRAIGHT; base_rotation = 90; }
                    else {
                        type = CORNER;
                        if (conns[0] == EAST && conns[1] == SOUTH) base_rotation = 90;
                        else if (conns[0] == SOUTH && conns[1] == WEST) base_rotation = 180;
                        else if (conns[0] == NORTH && conns[1] == WEST) base_rotation = 270;
                    }
                    break;
                case 3:
                    type = T_JUNCTION;
                    if (std::find(conns.begin(), conns.end(), SOUTH) == conns.end()) base_rotation = 0;
                    else if (std::find(conns.begin(), conns.end(), WEST) == conns.end()) base_rotation = 90;
                    else if (std::find(conns.begin(), conns.end(), NORTH) == conns.end()) base_rotation = 180;
                    else if (std::find(conns.begin(), conns.end(), EAST) == conns.end()) base_rotation = 270;
                    break;
                case 4:
                    type = CROSS;
                    break;
            }

            int random_rotation_steps = g() % 4;
            board.at(r, c) = Tile(type, base_rotation + random_rotation_steps * 90);
        }
    }

    // 4. Designate a power tile
    int power_r = g() % height;
    int power_c = g() % width;
    board.at(power_r, power_c).type = POWER;
    board.powerTile = {power_r, power_c};
}
