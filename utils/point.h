#pragma once
#ifndef POINT_H
#define POINT_H

#include <vector>

// Definition of the Point structure
struct Point {
    int x;
    int y;
    int r;
    int g;
    int b;
    float opacity;
};

// Function to read points from a CSV file
std::vector<Point> read_points_csv(const char* csv_file);

#endif // POINT_H
