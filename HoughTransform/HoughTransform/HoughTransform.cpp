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

    for (int y = 0; y < edges.height; y++) {
        for (int x = 0; x < edges.width; x++) {
            if (edges.data[y * edges.width + x] == 255) {
                for (int t = 0; t < result.thetaSize; t++) {
                    int rho = (int)round(x * cosTheta[t] + y * sinTheta[t]) + result.diag;
                    if (rho >= 0 && rho < result.rhoSize)
                        result.accumulator[rho * result.thetaSize + t]++;
                }
            }
        }
    }

    int maxVal = 0;
    for (int i = 0; i < (int)result.accumulator.size(); i++)
        if (result.accumulator[i] > maxVal)
            maxVal = result.accumulator[i];
    result.maxVal = maxVal;

    return result;
}