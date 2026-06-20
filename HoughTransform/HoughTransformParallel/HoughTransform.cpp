#include "HoughTransform.h"

static const double PI = acos(-1.0);
static const int THETA_BINS = 180;

HoughResult houghTransform(const Image& edges) {
    HoughResult result;
    result.thetaSize = THETA_BINS;
    result.diag = (int)ceil(sqrt((double)edges.width * edges.width + (double)edges.height * edges.height));
    result.rhoSize = 2 * result.diag;
    result.accumulator.assign(result.rhoSize * result.thetaSize, 0);

    vector<double> cosTheta(result.thetaSize);
    vector<double> sinTheta(result.thetaSize);
    for (int t = 0; t < result.thetaSize; t++) {
        double theta = t * PI / 180.0;
        cosTheta[t] = cos(theta);
        sinTheta[t] = sin(theta);
    }

    combinable<vector<int>> localAccumulator(
        [&]() {
            return vector<int>(result.rhoSize * result.thetaSize, 0);
        });

    parallel_for(blocked_range<int>(0, edges.height),
        [&](const blocked_range<int>& range) {
            auto& acc = localAccumulator.local();
            for (int y = range.begin(); y != range.end(); y++) {
                for (int x = 0; x < edges.width; x++) {
                    if (edges.data[y * edges.width + x] == 255) {
                        for (int t = 0; t < result.thetaSize; t++) {
                            int rho = (int)round(x * cosTheta[t] + y * sinTheta[t]) + result.diag;
                            if (rho >= 0 && rho < result.rhoSize)
                                acc[rho * result.thetaSize + t]++;
                        }
                    }
                }
            }
        });

    localAccumulator.combine_each([&](const vector<int>& local) {
            for (int i = 0; i < (int)result.accumulator.size(); i++) {
                result.accumulator[i] += local[i];
            }
        });

    result.maxVal = parallel_reduce(
        blocked_range<int>(0, (int)result.accumulator.size()), 0,
        [&](const blocked_range<int>& r, int localMax) {
            for (int i = r.begin(); i != r.end(); i++)
                if (result.accumulator[i] > localMax) localMax = result.accumulator[i];
            return localMax;
        },
        [](int a, int b) { return max(a, b); }
    );

    return result;
}