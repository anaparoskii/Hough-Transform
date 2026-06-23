#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "ImageLoader.h"
#include "EdgeDetector.h"
#include "HoughTransform.h"
#include "LineDetector.h"
#include <tbb/flow_graph.h>
#include <chrono>

using namespace std;
using namespace tbb;

struct ImageData {
    Image original;
    Image gray;
    Image edges;
    vector<Line> lines;
    HoughResult accumulator;
    string inputPath;
    string outputPath;
    double prepareImageTime;
    double detectEdgesTime;
    double houghTransformTime;
    double detectLinesTime;
};

void computeSpeedup(const map<string, double>& parallelTimes, ofstream& outputFile) {
    ifstream serialTimingFile("../HoughTransform/output/serial_times.txt");
    if (!serialTimingFile.is_open()) {
        outputFile << "\nSerial timing file not found - run serial program first.\n";
        return;
    }

    outputFile << "\nSpeedup (serial / parallel) -----\n";
    map<string, double> serialTimes;
    string line;
    while (getline(serialTimingFile, line)) {
        istringstream ss(line);
        string imageName;
        double time;
        if (ss >> imageName >> time)
            serialTimes[imageName] = time;
    }

    double totalSpeedup = 0.0;
    int count = 0;
    for (const auto& pair : parallelTimes) {
        auto it = serialTimes.find(pair.first);
        if (it != serialTimes.end() && pair.second > 0.0) {
            double speedup = it->second / pair.second;
            outputFile << "Image: " << pair.first
                       << " -> speedup: " << speedup << "x\n";
            totalSpeedup += speedup;
            count++;
        }
    }
    if (count > 0)
        outputFile << "Average speedup: " << (totalSpeedup / count) << "x\n";
}

int main()
{
    cout << "Hough Transform Parallel" << endl;

    map<string, string> images = {
        { "images/test1.png", "output/test1" },
        { "images/test2.png", "output/test2" },
        { "images/test3.png", "output/test3" },
        { "images/test4.bmp", "output/test4" }
    };

    ofstream outputFile("output/results.txt");
    outputFile << "Hough Transform Results Parallel -----\n";

    map<string, double> parallelTimes;
    flow::graph graph;

    flow::function_node<ImageData, ImageData> prepareImageNode(graph, flow::unlimited,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.original = loadImage(data.inputPath);
            data.gray = grayscaleConvert(data.original);
            auto end = chrono::high_resolution_clock::now();
            data.prepareImageTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [prepareImageNode]: " << data.prepareImageTime << endl;
            return data;
        });

    flow::function_node<ImageData, ImageData> detectEdgesNode(graph, flow::unlimited,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.edges = sobelEdgeDetect(data.gray);
            auto end = chrono::high_resolution_clock::now();
            data.detectEdgesTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [detectEdgesNode]: " << data.detectEdgesTime << endl;
            return data;
        });

    flow::function_node<ImageData, ImageData> houghTransformNode(graph, flow::unlimited,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.accumulator = houghTransform(data.edges);
            auto end = chrono::high_resolution_clock::now();
            data.houghTransformTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [houghTransformNode]: " << data.houghTransformTime << endl;
            return data;
        });

    flow::function_node<ImageData, ImageData> detectLinesNode(graph, flow::unlimited,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.lines = detectLines(data.accumulator);
            auto end = chrono::high_resolution_clock::now();
            data.detectLinesTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [detectLinesNode]: " << data.detectLinesTime << endl;
            return data;
        });

    flow::function_node<ImageData, ImageData> resultsNode(graph, flow::serial,
        [&outputFile, &parallelTimes](ImageData data) -> ImageData {
            double timeTotal =
                data.prepareImageTime + data.detectEdgesTime
                + data.houghTransformTime + data.detectLinesTime;

            outputFile << "Image: " << data.inputPath << "\n";
            outputFile << "Time - prepare image: " << data.prepareImageTime << "\n";
            outputFile << "Time - detect edges: " << data.detectEdgesTime << "\n";
            outputFile << "Time - hough transform: " << data.houghTransformTime << "\n";
            outputFile << "Time - detect lines: " << data.detectLinesTime << "\n";
            outputFile << "Time - total: " << timeTotal << "\n";
            outputFile << "Number of lines: " << data.lines.size() << "\n";

            Image imageLines = drawLines(data.original, data.lines);
            saveImage(data.outputPath + "_grayscale.png", data.gray);
            saveImage(data.outputPath + "_edges.png", data.edges);
            saveImage(data.outputPath + "_result.png", imageLines);
            saveImage(data.outputPath + "_accumulator.png", visualizeAccumulator(data.accumulator));

            cout << "Results saved!" << endl;

            parallelTimes[data.inputPath] = timeTotal;

            return data;
        });

    flow::make_edge(prepareImageNode, detectEdgesNode);
    flow::make_edge(detectEdgesNode, houghTransformNode);
    flow::make_edge(houghTransformNode, detectLinesNode);
    flow::make_edge(detectLinesNode, resultsNode);

    for (const auto& pair : images) {
        ImageData data;
        data.inputPath = pair.first;
        data.outputPath = pair.second;
        prepareImageNode.try_put(data);
    }

    graph.wait_for_all();

    computeSpeedup(parallelTimes, outputFile);

    cout << "Done" << endl;
}