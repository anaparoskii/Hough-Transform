#include "LineDetector.h"

static const double PI = acos(-1.0);
static const double ALPHA = 0.4;

vector<Line> detectLines(const HoughResult& result) {
    combinable<vector<Line>> localLines;
    vector<Line> detectedLines;

    parallel_for(blocked_range<int>(0, result.rhoSize),
        [&](const blocked_range<int>& range) {
            auto& lines = localLines.local();
            for (int r = range.begin(); r != range.end(); r++) {
                for (int t = 0; t < result.thetaSize; t++) {
                    int votes = result.accumulator[r * result.thetaSize + t];
                    int threshold = (int)(ALPHA * result.maxVal);
                    if (votes < threshold) continue;

                    if (!isLocalMaxima(r, t, votes, result)) continue;

                    Line line;
                    line.rho = r - result.diag;
                    line.theta = t * PI / result.thetaSize;
                    line.votes = votes;

                    lines.push_back(line);
                }
            }
        });

    localLines.combine_each([&](const vector<Line>& local) {
            detectedLines.insert(detectedLines.end(), local.begin(), local.end());
        });

    return detectedLines;
}

bool isLocalMaxima(int rhoIdx, int thetaIdx, int votes, const HoughResult& result) {
    for (int dr = -4; dr <= 4; dr++) {
        for (int dt = -4; dt <= 4; dt++) {
            if (dr == 0 && dt == 0) continue;

            int nr = rhoIdx + dr;
            int nt = (thetaIdx + dt + result.thetaSize) % result.thetaSize;

            if (nr < 0 || nr >= result.rhoSize) continue;

            if (result.accumulator[nr * result.thetaSize + nt] > votes) return false;
        }
    }
    return true;
}