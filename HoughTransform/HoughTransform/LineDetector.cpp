#include "LineDetector.h"

static const double PI = acos(-1.0);
static const double ALPHA = 0.4;

vector<Line> detectLines(const HoughResult& result) {
    vector<Line> detectedLines;

    for (int r = 0; r < result.rhoSize; r++) {
        for (int t = 0; t < result.thetaSize; t++) {
            int votes = result.accumulator[r * result.thetaSize + t];
            int threshold = (int)(ALPHA * result.maxVal);
            if (votes < threshold) continue;

            if (!isLocalMaxima(r, t, votes, result)) continue;
            
            Line line;
            line.rho = r - result.diag;
            line.theta = t * PI / result.thetaSize;
            line.votes = votes;

            detectedLines.push_back(line);
        }
    }

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