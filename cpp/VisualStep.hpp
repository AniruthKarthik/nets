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
    string type; // "TRY", "UNDO", "SUCCESS", "FAIL", "CONSIDER", "SCORE", "REGION"
    double score;
    int upmask = 0;
    int leftreq = 0;
    int r0 = -1, r1 = -1, c0 = -1, c1 = -1; // Region bounds for DAC

    json to_json() const {
        return {
            {"row", row},
            {"col", col},
            {"rotation", rotation},
            {"type", type},
            {"score", score},
            {"upmask", upmask},
            {"leftreq", leftreq},
            {"r0", r0}, {"r1", r1}, {"c0", c0}, {"c1", c1}
        };
    }
};

#endif // VISUAL_STEP_HPP
