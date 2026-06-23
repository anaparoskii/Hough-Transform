#pragma once
#include <vector>
#include "ImageLoader.h"
#include <cmath>

using namespace std;

struct HoughResult {
	vector<int> accumulator;
	int rhoSize;
	int thetaSize;
	int diag;
	int maxVal;
};

HoughResult houghTransform(const Image& edges);
Image visualizeAccumulator(const HoughResult& result);
