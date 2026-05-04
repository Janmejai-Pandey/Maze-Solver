#include "ImageExtractor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>

cv::Mat ImageExtractor::lastProcessed_;
cv::Mat ImageExtractor::lastThreshold_;

cv::Mat ImageExtractor::getLastProcessedImage() { return lastProcessed_; }
cv::Mat ImageExtractor::getLastThresholdImage() { return lastThreshold_; }

// ── Preprocess: grayscale, blur, threshold ────────────────────
cv::Mat ImageExtractor::preprocessImage(const cv::Mat& input) {
    cv::Mat gray, blurred, binary;

    // Convert to grayscale
    if (input.channels() == 3 || input.channels() == 4)
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else
        gray = input.clone();

    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    // Adaptive threshold for handling uneven lighting
    cv::adaptiveThreshold(
        blurred, binary, 255,
        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        cv::THRESH_BINARY,
        15,   // block size
        10    // constant subtracted
    );

    // Morphological operations to clean up
    cv::Mat kernel = cv::getStructuringElement(
        cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);

    return binary;
}

// ── Convert binary image to grid ──────────────────────────────
std::vector<std::vector<char>> ImageExtractor::imageToGrid(
    const cv::Mat& binary, int rows, int cols)
{
    std::vector<std::vector<char>> grid(
        rows, std::vector<char>(cols, '#'));

    float cellH = (float)binary.rows / rows;
    float cellW = (float)binary.cols / cols;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // Sample the center region of each cell
            int y1 = (int)(r * cellH + cellH * 0.2f);
            int y2 = (int)(r * cellH + cellH * 0.8f);
            int x1 = (int)(c * cellW + cellW * 0.2f);
            int x2 = (int)(c * cellW + cellW * 0.8f);

            y1 = std::max(0, std::min(y1, binary.rows - 1));
            y2 = std::max(0, std::min(y2, binary.rows - 1));
            x1 = std::max(0, std::min(x1, binary.cols - 1));
            x2 = std::max(0, std::min(x2, binary.cols - 1));

            if (y2 <= y1 || x2 <= x1) continue;

            cv::Mat cell = binary(cv::Rect(x1, y1, x2 - x1, y2 - y1));

            // Count white pixels (path = white, wall = black)
            int whitePixels = cv::countNonZero(cell);
            int totalPixels = cell.rows * cell.cols;
            float whiteRatio = (float)whitePixels / totalPixels;

            // If more than 50% white -> open path, else wall
            grid[r][c] = (whiteRatio > 0.5f) ? ' ' : '#';
        }
    }

    return grid;
}

// ── Auto-detect start and end points ──────────────────────────
void ImageExtractor::autoDetectStartEnd(
    std::vector<std::vector<char>>& grid, int rows, int cols)
{
    // Strategy: find open cells on the borders
    // Top/bottom border -> start
    // Right/left border -> end

    Point start = {-1, -1}, end = {-1, -1};

    // Scan borders for open cells
    std::vector<Point> borderOpenings;

    // Top and bottom rows
    for (int c = 0; c < cols; c++) {
        if (grid[0][c] == ' ')      borderOpenings.push_back({0, c});
        if (grid[rows-1][c] == ' ') borderOpenings.push_back({rows-1, c});
    }
    // Left and right columns
    for (int r = 0; r < rows; r++) {
        if (grid[r][0] == ' ')      borderOpenings.push_back({r, 0});
        if (grid[r][cols-1] == ' ') borderOpenings.push_back({r, cols-1});
    }

    if (borderOpenings.size() >= 2) {
        // Use first and last border openings
        start = borderOpenings.front();
        end   = borderOpenings.back();
    } else if (borderOpenings.size() == 1) {
        start = borderOpenings[0];
        // Find farthest open cell from start
        int maxDist = 0;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid[r][c] == ' ') {
                    int dist = std::abs(r - start.row) + std::abs(c - start.col);
                    if (dist > maxDist) {
                        maxDist = dist;
                        end = {r, c};
                    }
                }
            }
        }
    } else {
        // No border openings: use top-left-most and bottom-right-most open cells
        for (int r = 0; r < rows && start.row == -1; r++)
            for (int c = 0; c < cols && start.row == -1; c++)
                if (grid[r][c] == ' ') start = {r, c};

        for (int r = rows - 1; r >= 0 && end.row == -1; r--)
            for (int c = cols - 1; c >= 0 && end.row == -1; c--)
                if (grid[r][c] == ' ' && !(r == start.row && c == start.col))
                    end = {r, c};
    }

    if (start.row != -1) grid[start.row][start.col] = 'S';
    if (end.row != -1)   grid[end.row][end.col]     = 'E';
}

// ── Auto-detect grid size from image ──────────────────────────
static int detectGridSize(const cv::Mat& binary, bool horizontal) {
    // Project onto one axis, look for periodic pattern
    int len = horizontal ? binary.cols : binary.rows;
    std::vector<int> profile(len, 0);

    for (int i = 0; i < len; i++) {
        int count = 0;
        if (horizontal) {
            for (int r = 0; r < binary.rows; r++)
                if (binary.at<uchar>(r, i) == 0) count++;
        } else {
            for (int c = 0; c < binary.cols; c++)
                if (binary.at<uchar>(i, c) == 0) count++;
        }
        profile[i] = count;
    }

    // Find average spacing between dark lines (walls)
    int threshold = (horizontal ? binary.rows : binary.cols) / 3;
    std::vector<int> wallPositions;
    bool inWall = false;

    for (int i = 0; i < len; i++) {
        if (profile[i] > threshold && !inWall) {
            wallPositions.push_back(i);
            inWall = true;
        } else if (profile[i] <= threshold) {
            inWall = false;
        }
    }

    if (wallPositions.size() < 2) return 0;

    // Average gap between walls
    int totalGap = 0;
    for (size_t i = 1; i < wallPositions.size(); i++)
        totalGap += wallPositions[i] - wallPositions[i-1];
    int avgGap = totalGap / (int)(wallPositions.size() - 1);

    if (avgGap <= 0) return 0;
    return len / avgGap;
}

// ── Main extraction function ──────────────────────────────────
Maze ImageExtractor::extractFromImage(
    const std::string& imagePath, int targetRows, int targetCols)
{
    cv::Mat image = cv::imread(imagePath);
    if (image.empty()) {
        std::cerr << "Failed to load image: " << imagePath << "\n";
        return Maze();
    }
    return extractFromMat(image, targetRows, targetCols);
}

Maze ImageExtractor::extractFromMat(
    const cv::Mat& image, int targetRows, int targetCols)
{
    if (image.empty()) return Maze();

    // Store original for debug
    lastProcessed_ = image.clone();

    // Preprocess
    cv::Mat binary = preprocessImage(image);
    lastThreshold_ = binary.clone();

    // Auto-detect grid dimensions if not provided
    if (targetRows <= 0 || targetCols <= 0) {
        int detectedRows = detectGridSize(binary, false);
        int detectedCols = detectGridSize(binary, true);

        // Clamp to reasonable values
        targetRows = (detectedRows > 3 && detectedRows < 200)
                     ? detectedRows : 21;
        targetCols = (detectedCols > 3 && detectedCols < 200)
                     ? detectedCols : 21;

        // Ensure odd for proper maze structure
        if (targetRows % 2 == 0) targetRows++;
        if (targetCols % 2 == 0) targetCols++;

        std::cout << "Auto-detected grid: "
                  << targetRows << "x" << targetCols << "\n";
    }

    // Convert to grid
    auto grid = imageToGrid(binary, targetRows, targetCols);

    // Auto-detect start/end
    autoDetectStartEnd(grid, targetRows, targetCols);

    // Build maze string representation
    std::vector<std::string> lines(targetRows);
    for (int r = 0; r < targetRows; r++) {
        lines[r].resize(targetCols);
        for (int c = 0; c < targetCols; c++)
            lines[r][c] = grid[r][c];
    }

    return Maze(lines);
}