#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;
using namespace tbb;

struct Image {
	int width;
	int height;
	int channels;
	vector<unsigned char> data;
};

Image loadImage(const string& path);
Image grayscaleConvert(const Image& image);
void saveImage(const string& path, const Image& image);