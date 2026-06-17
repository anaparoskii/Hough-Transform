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
	vector<pair<double, double>> lines;
};

HoughResult houghTransform(const Image& edges);
