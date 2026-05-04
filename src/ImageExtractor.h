#pragma once
#include "Maze.h"
#include <string>
#include <opencv2/core.hpp>

class ImageExtractor {
public:
    // Extract maze grid from an image file
    static Maze extractFromImage(const std::string& imagePath,
                                 int targetRows = 0,
                                 int targetCols = 0);

    // Extract from an OpenCV Mat
    static Maze extractFromMat(const cv::Mat& image,
                               int targetRows = 0,
                               int targetCols = 0);

    // Get the processed debug image (for display)
    static cv::Mat getLastProcessedImage();
    static cv::Mat getLastThresholdImage();

private:
    static cv::Mat lastProcessed_;
    static cv::Mat lastThreshold_;

    static std::vector<std::vector<char>> imageToGrid(
        const cv::Mat& binary, int rows, int cols);

    static void autoDetectStartEnd(
        std::vector<std::vector<char>>& grid,
        int rows, int cols);

    static cv::Mat preprocessImage(const cv::Mat& input);
};