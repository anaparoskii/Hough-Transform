#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageLoader.h"

Image loadImage(const string& path) {
	Image img;
	unsigned char* rawImage = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, 0);
	if (!rawImage)
		throw runtime_error("Error loading image: " + path + " - " + stbi_failure_reason());
	int totalBytes = img.width * img.height * img.channels;
	img.data.assign(rawImage, rawImage + totalBytes);
	stbi_image_free(rawImage);
	return img;
}

Image grayscaleConvert(const Image& image) {
	if (image.channels == 1) return image;
	Image gray;
	gray.width = image.width;
	gray.height = image.height;
	gray.channels = 1;
	gray.data.resize(image.width * image.height);

	parallel_for(blocked_range<int>(0, gray.height),
		[&](const blocked_range<int>& range) {
			for (int y = range.begin(); y != range.end(); y++)
			{
				for (int x = 0; x < image.width; x++)
				{
					int pixelOffset = y * image.width + x;
					int colorIdx = pixelOffset * image.channels;
					int grayIdx = pixelOffset;

					unsigned char r = image.data[colorIdx];
					unsigned char g = image.data[colorIdx + 1];
					unsigned char b = image.data[colorIdx + 2];

					gray.data[grayIdx] = static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);
				}
			}
		});

	return gray;
}

void saveImage(const string& path, const Image& image) {
	int result = stbi_write_png(path.c_str(), image.width, image.height, image.channels, image.data.data(), image.width * image.channels);

	if (!result) throw runtime_error("Error saving image: " + path);
}