#include <iostream>
#include <string>
#include <map>
#include "ImageLoader.h"
#include "EdgeDetector.h"
#include "HoughTransform.h"
#include "LineDetector.h"
#include <chrono>

using namespace std;

void processImage(const string& input, const string& output) {
    auto start = chrono::high_resolution_clock::now();
    Image img = loadImage(input);
    Image gray = grayscaleConvert(img);
    auto end = chrono::high_resolution_clock::now();
    saveImage(output + "_grayscale.png", gray);
    double timePrepare = chrono::duration<double, std::milli>(end - start).count();
    cout << "Time [prepareImage]: " << timePrepare << endl;

    start = chrono::high_resolution_clock::now();
    Image edges = sobelEdgeDetect(gray);
    end = chrono::high_resolution_clock::now();
    double timeDetectEdges = chrono::duration<double, std::milli>(end - start).count();
    cout << "Time [detectEdges]: " << timeDetectEdges << endl;

    start = chrono::high_resolution_clock::now();
    HoughResult hough = houghTransform(edges);
    end = chrono::high_resolution_clock::now();
    double timeHoughTransform = chrono::duration<double, std::milli>(end - start).count();
    cout << "Time [houghTransform]: " << timeHoughTransform << endl;

    start = chrono::high_resolution_clock::now();
    vector<Line> lines = detectLines(hough);
    end = chrono::high_resolution_clock::now();
    double timeDetectLines = chrono::duration<double, std::milli>(end - start).count();
    cout << "Time [detectLines]: " << timeDetectLines << endl;

    double timeTaken = timePrepare + timeDetectEdges + timeHoughTransform + timeDetectLines;
    cout << "Time taken: " << timeTaken << endl;
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