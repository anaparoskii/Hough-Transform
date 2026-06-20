#pragma once
#include <vector>
#include "HoughTransform.h"

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/combinable.h>

using namespace std;
using namespace tbb;

struct Line {
	double rho;
	double theta;
	int votes;
};

vector<Line> detectLines(const HoughResult& result);
bool isLocalMaxima(int rho, int theta, int votes, const HoughResult& result);