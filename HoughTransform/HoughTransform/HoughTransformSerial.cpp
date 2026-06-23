#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "ImageLoader.h"
#include "EdgeDetector.h"
#include "HoughTransform.h"
#include "LineDetector.h"
#include <chrono>

using namespace std;

double processImage(const string& input, const string& output, ofstream& outputFile) {
    auto start = chrono::high_resolution_clock::now();
    Image img = loadImage(input);
    Image gray = grayscaleConvert(img);
    auto end = chrono::high_resolution_clock::now();
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

    double timeTotal = timePrepare + timeDetectEdges + timeHoughTransform + timeDetectLines;

    outputFile << "Image: " << input << "\n";
    outputFile << "Time - prepare image: " << timePrepare << "\n";
    outputFile << "Time - detect edges: " << timeDetectEdges << "\n";
    outputFile << "Time - hough transform: " << timeHoughTransform << "\n";
    outputFile << "Time - detect lines: " << timeDetectLines << "\n";
    outputFile << "Time - total: " << timeTotal << "\n";
    outputFile << "Number of lines: " << lines.size() << "\n";

    Image imageLines = drawLines(img, lines);
    saveImage(output + "_grayscale.png", gray);
    saveImage(output + "_edges.png", edges);
    saveImage(output + "_result.png", imageLines);
    saveImage(output + "_accumulator.png", visualizeAccumulator(hough));

    cout << "Results saved!" << endl;
    return timeTotal;
}

int main()
{
    cout << "Hough Transform Serial" << endl;

    map<string, string> images = {
        { "images/test1.png", "output/test1" },
        { "images/test2.png", "output/test2" },
        { "images/test3.png", "output/test3" },
        { "images/test4.bmp", "output/test4" }
    };

    ofstream outputFile("output/results.txt");
    outputFile << "Hough Transform Results Serial -----\n";

    ofstream timingFile("output/serial_times.txt");

    for (const auto& pair : images) {
        try {
            double total = processImage(pair.first, pair.second, outputFile);
            timingFile << pair.first << " " << total << "\n";
        }
        catch (const exception& e) {
            cerr << "Failed: " << e.what() << endl;
        }
    }

    cout << "Done" << endl;
}