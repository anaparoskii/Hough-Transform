#include <iostream>
#include <string>
#include <map>
#include "ImageLoader.h"
#include <chrono>

using namespace std;

void processImage(const string& input, const string& output) {
    auto start = chrono::high_resolution_clock::now();
    Image img = loadImage(input);
    Image gray = grayscaleConvert(img);
    auto end = chrono::high_resolution_clock::now();
    saveImage(output + "_grayscale.png", gray);
    double time = chrono::duration<double, std::milli>(end - start).count();
    cout << "Time [prepareImageNode]: " << time << endl;
}

int main()
{
    cout << "Hough Transform Serial" << endl;

    map<string, string> images = {
        { "images/test1.bmp", "output/test1" },
        { "images/test2.png", "output/test2" },
        { "images/test3.png", "output/test3" },
        { "images/test4.bmp", "output/test4" },
        { "images/test5.png", "output/test5" },
        { "images/test6.png", "output/test6" }
    };

    for (const auto& pair : images) {
        try {
            processImage(pair.first, pair.second);
        }
        catch (const exception& e) {
            cerr << "Failed: " << e.what() << endl;
        }
    }

    cout << "Done" << endl;
}