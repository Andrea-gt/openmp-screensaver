#include "point.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <omp.h>

std::vector<Point> read_points_csv(const char* csv_file) {
    std::ifstream file(csv_file);
    if (!file.is_open()) {
        std::cerr << "Error opening the CSV file" << std::endl;
        return {};
    }

    // Read headers
    std::string header;
    std::getline(file, header);

    // Read all lines from the file
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    // Allocate memory for points
    std::vector<Point> points;
    points.reserve(lines.size());

    // Process lines in parallel
    #pragma omp parallel
    {
        std::vector<Point> local_points;
        local_points.reserve(lines.size());
        #pragma omp for nowait
        for (int i = 0; i < lines.size(); ++i) {
            std::istringstream stream(lines[i]);
            Point point;
            char comma;
            if (stream >> point.x >> comma >> point.y >> comma >> point.r >> comma >> point.g >> comma >> point.b >> comma >> point.opacity) {
                local_points.push_back(point);
            }
        }

        // Merge local results into the global vector
        #pragma omp critical
        {
            points.insert(points.end(), local_points.begin(), local_points.end());
        }
    }

    return points;
}
