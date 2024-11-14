#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

struct SphereParams {
    int center_x;
    int center_y;
    int radius;
};

struct Point3D {
    double x;
    double y;
    double z;
};

SphereParams readSphereParams(const string& filename) {
    ifstream ifs(filename);
    SphereParams params;
    ifs >> params.center_x >> params.center_y >> params.radius;
    return params;
}

// Find the brightest pixel in the image
void findBrightestPixel(const Image& image, int& max_x, int& max_y, int& max_intensity) {
    max_intensity = -1;
    
    for (int i = 0; i < image.num_rows(); ++i) {
        for (int j = 0; j < image.num_columns(); ++j) {
            int pixel = image.GetPixel(i, j);
            if (pixel > max_intensity) {
                max_intensity = pixel;
                max_x = i;
                max_y = j;
            }
        }
    }
}

// Calculate surface normal at given point on sphere
Point3D calculateNormal(int x, int y, const SphereParams& params) {
    // Convert image coordinates to sphere coordinates
    double dx = x - params.center_x;
    double dy = y - params.center_y;
    
    // Using the sphere equation: x^2 + y^2 + z^2 = r^2
    // Calculate z component
    double z = sqrt(params.radius * params.radius - dx*dx - dy*dy);
    
    // Create normalized vector
    double length = sqrt(dx*dx + dy*dy + z*z);
    
    Point3D normal;
    normal.x = dx / length;
    normal.y = dy / length;
    normal.z = z / length;
    
    return normal;
}

void writeLightDirections(const vector<Point3D>& directions, const string& filename) {
    ofstream ofs(filename);
    for (const auto& dir : directions) {
        ofs << dir.x << " " << dir.y << " " << dir.z << endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("Usage: %s {input parameters filename} {sphere image 1} {sphere image 2} {sphere image 3} {output directions filename}\n", argv[0]);
        return 0;
    }
    
    // Read command line arguments
    const string params_file(argv[1]);
    const string sphere_files[] = {argv[2], argv[3], argv[4]};
    const string output_file(argv[5]);
    
    // Read sphere parameters
    SphereParams sphere_params = readSphereParams(params_file);
    
    // Process each sphere image
    vector<Point3D> light_directions;
    
    for (int i = 0; i < 3; ++i) {
        Image sphere_image;
        if (!ReadImage(sphere_files[i], &sphere_image)) {
            cout << "Can't open file " << sphere_files[i] << endl;
            return 0;
        }
        
        // Find brightest pixel
        int max_x, max_y, max_intensity;
        findBrightestPixel(sphere_image, max_x, max_y, max_intensity);
        
        // Calculate normal at brightest point
        Point3D normal = calculateNormal(max_x, max_y, sphere_params);
        
        // Scale normal by intensity
        normal.x *= max_intensity;
        normal.y *= max_intensity;
        normal.z *= max_intensity;
        
        light_directions.push_back(normal);
    }
    
    // Write results to file
    writeLightDirections(light_directions, output_file);
    
    return 0;
}
