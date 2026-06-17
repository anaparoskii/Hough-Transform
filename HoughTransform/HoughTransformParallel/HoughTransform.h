#pragma once
#include <vector>
#include "ImageLoader.h"

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/combinable.h>

using namespace std;
using namespace tbb;

struct HoughResult {
	vector<int> accumulator;
	int rhoSize;
	int thetaSize;
	int diag;
	vector<pair<double, double>> lines;
};

HoughResult houghTransform(const Image& edges);
