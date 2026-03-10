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
    int upmask;
    int leftreq;

    json to_json() const {
        return {
            {"row", row},
            {"col", col},
            {"rotation", rotation},
            {"type", type},
            {"score", score},
            {"upmask", upmask},
            {"leftreq", leftreq}
        };
    }
};

#endif // VISUAL_STEP_HPP
