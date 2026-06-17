#pragma once
#include "ImageLoader.h"
#include <cmath>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

using namespace std;
using namespace tbb;

Image sobelEdgeDetect(const Image& image);