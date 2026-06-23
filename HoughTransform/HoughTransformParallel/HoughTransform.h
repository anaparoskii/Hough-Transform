#pragma once
#include <vector>
#include "ImageLoader.h"

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/combinable.h>
#include <tbb/parallel_reduce.h>

using namespace std;
using namespace tbb;

struct HoughResult {
	vector<int> accumulator;
	int rhoSize;
	int thetaSize;
	int diag;
	int maxVal;
};

HoughResult houghTransform(const Image& edges);
Image visualizeAccumulator(const HoughResult& result);
