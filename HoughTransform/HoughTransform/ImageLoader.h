#pragma once
#include <vector>
#include <string>
#include <stdexcept>

#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

struct Image {
	int width;
	int height;
	int channels;
	vector<unsigned char> data;
};

Image loadImage(const string& path);
Image grayscaleConvert(const Image& image);
void saveImage(const string& path, const Image& image);