#include "EdgeDetector.h"

Image sobelEdgeDetect(const Image& img) {
    Image result;
    result.height = img.height;
    result.width = img.width;
    result.channels = 1;
    result.data.resize(img.width * img.height, 0);

    const int Gx[3][3] = {
       { -1,  0,  1 },
       { -2,  0,  2 },
       { -1,  0,  1 }
    };

    const int Gy[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    parallel_for(blocked_range<int>(1, img.height - 1),
        [&](const blocked_range<int>& range) {
            for (int y = range.begin(); y != range.end(); y++) {
                for (int x = 1; x < img.width - 1; x++) {
                    int gx = 0;
                    int gy = 0;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            int pixel = img.data[(y + ky) * img.width + (x + kx)];
                            gx += Gx[ky + 1][kx + 1] * pixel;
                            gy += Gy[ky + 1][kx + 1] * pixel;
                        }
                    }

                    int magnitude = static_cast<int>(sqrt(gx * gx + gy * gy));
                    result.data[y * img.width + x] = (magnitude > 100) ? 255 : 0;
                }
            }
        });

    return result;
}