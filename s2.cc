/**
 * Daniel Kaijzer
 * 
 * Compute normal vector to the spheres surface at a given point
 * knowing the points cooridnates (in image coordinate frame)
 * and coordinates of the center and redius of spheres projection onto the image plane 
 * (assume orthonormal projection)
 * 
 * Steps:
 *  1. Read sphere params (centroid and radius) and read 3 sphere images
 *  2. For each sphere image
 *      Find brightest pixel location
 *      Calculate surface normal at that point using sphere geometry
 *      Scale the normal vector by the brightness value 
 *      all to determine light source direction and intensity
 * 3. Write three lines to output, each containing x,y,z of light source vector
 */




#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

struct SphereParam{
    int xbar;
    int ybar;
    int radius;
};

struct Vector3D{
    double x;
    double y;
    double z;
};


SphereParam readParams(const string& filename){
    ifstream ifs(filename);
    SphereParam params;
    // ifs >> params.xbar >> params.ybar >> params.radius;
    ifs >> params.ybar >> params.xbar >> params.radius; // reversed to match professor's output
    return params;
}

/**
 * Finds brightest pixel in an image
 * updates params for brightest pixel (location and intensity value)
 */
void findBrightestPixel(const Image* image, int& max_x, int& max_y, int& max_intensity){
    max_intensity = -1;
    
    for (int i = 0; i < image->num_rows(); ++i){
        for (int j = 0; j < image->num_columns(); ++j){

            int pixel = image->GetPixel(i, j);

            if (pixel > max_intensity){
                max_intensity = pixel;
                max_x = i;
                max_y = j;
            }
        }
    }
}



/**
 * Calculates normal at given point on sphere (we pass in coords of brightest pixel)
 * using the following formula :
 *  Given the brightest point (x,y) on sphere's projected image, centroid (xbar, ybar) and radius r:

	light source direction = normal vector = (dx/length, dy/length, z/length)
	light source vector = normal vector * I
	
	where:
		# dx and dy are vectors from center to point x,y
		dx = x - xbar
		dy = y - ybar

		# z is calculated by rearranging sphere equation
        # since we're doing ortographic projection we can think of z as constant
		z = root(r^2 - dx^2 - dy^2)

		# use length to normalize to unit length
		length = root(dx^2 + dy^2 + z^2)

		I is intensity of brightest pixel
 */
Vector3D calculateNormal(int x, int y, const SphereParam& params){

    // calculate distance from centroid to brightest pixel
    double dx = x - params.xbar;
    double dy = y - params.ybar;
    
    // to find z, rearrange sphere equation: r^2 = x^2 + y^2 + z^2
    double z = sqrt(params.radius * params.radius - dx*dx - dy*dy);
    
    // calculate lenght so we can make normal a unit normal
    double length = sqrt(dx*dx + dy*dy + z*z);
    
    Vector3D normal;

    normal.x = dx / length;
    normal.y = dy / length;
    normal.z = z / length;
    
    return normal;
}


/**
 * write the output (normal of brgithest point directions) to text file
 */
void writeLightDirections(const vector<Vector3D>& directions, const string& filename){
    ofstream ofs(filename);

    for (const auto& dir : directions){
        // ofs << dir.x << " " << dir.y << " " << dir.z << endl;
        ofs << dir.y << " " << dir.x << " " << dir.z << endl; // switched x and y to match professor's output
    }
}





int main(int argc, char **argv){

    if (argc != 6) {
        printf("Usage: %s {input parameters filename} {sphere image 1} {sphere image 2} {sphere image 3} {output directions filename}\n", argv[0]);
        return 0;
    }
    
    const string params_file(argv[1]);
    const string sphere_files[] = {argv[2], argv[3], argv[4]};
    const string output_file(argv[5]);
    
    SphereParam sphere_params = readParams(params_file); // centroid and radius of sphere (from s1)
    

    // Process each image of sphere (3 total)
    vector<Vector3D> light_directions;

    for (int i = 0; i < 3; ++i){

        Image sphere_image;

        // always check if image is valid!
        if (!ReadImage(sphere_files[i], &sphere_image)){
            cout << "Can't open file " << sphere_files[i] << endl;
            return 0;
        }
        
        // Find brightest pixel, (pass these following variables in by reference)
        int max_x, max_y, max_intensity;
        findBrightestPixel(&sphere_image, max_x, max_y, max_intensity);
        
        // calculate normal at brightest point using sphere params from s1 and location of brightest pixel
        Vector3D normal = calculateNormal(max_x, max_y, sphere_params);
        
        // Scale normal by intensity so it captures both direction and magnitude
        normal.x *= max_intensity;
        normal.y *= max_intensity;
        normal.z *= max_intensity;
        
        light_directions.push_back(normal);
    }
    
    // Write results to file
    writeLightDirections(light_directions, output_file);
    
    return 0;
}