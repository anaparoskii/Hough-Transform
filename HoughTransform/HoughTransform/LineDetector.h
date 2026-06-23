#pragma once
#include <vector>
#include "HoughTransform.h"

using namespace std;

struct Line {
	double rho;
	double theta;
	int votes;
};

vector<Line> detectLines(const HoughResult& result);
bool isLocalMaxima(int rho, int theta, int votes, const HoughResult& result);
Image drawLines(const Image& originalImage, const std::vector<Line>& lines);