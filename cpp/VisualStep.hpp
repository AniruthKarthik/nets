#ifndef VISUAL_STEP_HPP
#define VISUAL_STEP_HPP

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

struct VisualStep {
    int row;
    int col;
    int rotation;
    string type; // "TRY", "UNDO", "SUCCESS", "FAIL", "CONSIDER", "SCORE"
    double score;

    json to_json() const {
        return {
            {"row", row},
            {"col", col},
            {"rotation", rotation},
            {"type", type},
            {"score", score}
        };
    }
};

#endif // VISUAL_STEP_HPP
