#include "LineDetector.h"

static const double PI = acos(-1.0);
static const double ALPHA = 0.5;
static const double MIN_LINE_FRACTION = 0.15;

vector<Line> detectLines(const HoughResult& result) {
    vector<Line> detectedLines;

    int threshold = max((int)(ALPHA * result.maxVal),
                        (int)(MIN_LINE_FRACTION * result.diag));

    for (int r = 0; r < result.rhoSize; r++) {
        for (int t = 0; t < result.thetaSize; t++) {
            int votes = result.accumulator[r * result.thetaSize + t];
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
    for (int dr = -6; dr <= 6; dr++) {
        for (int dt = -6; dt <= 6; dt++) {
            if (dr == 0 && dt == 0) continue;

            int nr = rhoIdx + dr;
            int nt = (thetaIdx + dt + result.thetaSize) % result.thetaSize;

            if (nr < 0 || nr >= result.rhoSize) continue;

            int neighborVal = result.accumulator[nr * result.thetaSize + nt];
            if (neighborVal > votes) return false;
            if (neighborVal == votes &&
                (nr * result.thetaSize + nt) < (rhoIdx * result.thetaSize + thetaIdx))
                return false;
        }
    }
    return true;
}

Image drawLines(const Image& originalImage, const std::vector<Line>& lines) {
    Image result;
    result.width = originalImage.width;
    result.height = originalImage.height;
    result.channels = 3;
    result.data.resize(result.width * result.height * 3);

    for (int y = 0; y < originalImage.height; y++) {
        for (int x = 0; x < originalImage.width; x++) {
            unsigned char val = originalImage.channels == 1
                ? originalImage.data[y * originalImage.width + x]
                : originalImage.data[(y * originalImage.width + x) * originalImage.channels];
            int idx = (y * result.width + x) * 3;
            result.data[idx] = val;
            result.data[idx + 1] = val;
            result.data[idx + 2] = val;
        }
    }

    for (const Line& line : lines) {
        double cosT = cos(line.theta);
        double sinT = sin(line.theta);

        if (fabs(sinT) > fabs(cosT)) {
            for (int x = 0; x < result.width; x++) {
                int y = (int)round((line.rho - x * cosT) / sinT);
                if (y < 0 || y >= result.height) continue;
                int idx = (y * result.width + x) * 3;
                result.data[idx] = 255;
                result.data[idx + 1] = 0;
                result.data[idx + 2] = 0;
            }
        } else {
            for (int y = 0; y < result.height; y++) {
                int x = (int)round((line.rho - y * sinT) / cosT);
                if (x < 0 || x >= result.width) continue;
                int idx = (y * result.width + x) * 3;
                result.data[idx] = 255;
                result.data[idx + 1] = 0;
                result.data[idx + 2] = 0;
            }
        }
    }

    return result;
}

