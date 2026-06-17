#include <iostream>
#include <string>
#include <map>
#include "ImageLoader.h"
#include "EdgeDetector.h"
#include <tbb/flow_graph.h>
#include <chrono>

using namespace std;
using namespace tbb;

struct ImageData {
    Image original;
    Image gray;
    Image edges;
    string inputPath;
    string outputPath;
    double prepareImageTime;
    double detectEdgesTime;
};

void processImage(const string& input, const string& output) {
    flow::graph graph;

    flow::function_node<ImageData, ImageData> prepareImageNode(graph, flow::serial,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.original = loadImage(data.inputPath);
            data.gray = grayscaleConvert(data.original);
            auto end = chrono::high_resolution_clock::now();
            saveImage(data.outputPath + "_grayscale.png", data.gray);
            data.prepareImageTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [prepareImageNode]: " << data.prepareImageTime << endl;
            return data;
        });

    flow::function_node<ImageData, ImageData> detectEdgesNode(graph, flow::serial,
        [](ImageData data) -> ImageData {
            auto start = chrono::high_resolution_clock::now();
            data.edges = sobelEdgeDetect(data.gray);
            auto end = chrono::high_resolution_clock::now();
            data.detectEdgesTime = chrono::duration<double, std::milli>(end - start).count();
            cout << "Time [detectEdgesNode]: " << data.detectEdgesTime << endl;
            return data;
        });

    flow::make_edge(prepareImageNode, detectEdgesNode);

    ImageData inputData;
    inputData.inputPath = input;
    inputData.outputPath = output;

    prepareImageNode.try_put(inputData);
    graph.wait_for_all();
}

int main()
{
    cout << "Hough Transform Parallel" << endl;

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