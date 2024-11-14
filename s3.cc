/**
 * Daniel Kaijzer
 * 
 * Notes:
 *  lambertian reflectance model:
 *      I = p(S*N)
 *          I: intensity
 *          p is albedo
 *          S = light direction
 *          N = surface normal
 * 
 * Steps:
 * 1. Input
 *      Read light source directions/intensities from s2 output
 *      read 3 obj images
 *      get step and threshold params
 * 2. For each valid pixel (brightness > threshold in all images)
 *      Set up the linear system using light directions and intensities
 *      Solve for surface normal and albedo
 *      Scale and store results
 * 3. Outputs
 *      Normals image
 *      Scale and create Albedo image 
 * 
 * Albedo image is basically like a "material map"
 * Albedo represents the surface's reflective propety
 *      
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

struct Vector3D{
    double x;
    double y;
    double z;
};

// Read light source directions and intensities from file
vector<Vector3D> readLightDirections(const string& filename){
    vector<Vector3D> directions;
    ifstream ifs(filename);
    string line;
    
    while (getline(ifs, line)) {
        istringstream iss(line);
        Vector3D dir;
        iss >> dir.y >> dir.x >> dir.z;
        directions.push_back(dir);
    }
    
    return directions;
}

/**
 * Assume that a pixel (x, y) is visible from all 3 light sources if its brightness in all 3 images is greater than a certain threshold. 
 * Check if pixel is above threshold!
 * threshold supplied as input
 * */
bool isPixelVisible(int x, int y, const vector<Image>& images, int threshold){

    for (const auto& an_image : images){
        if (an_image.GetPixel(x, y) <= threshold){
            return false;
        }
    }
    return true;
}




/**
 * 
 * Use given parametrs (light direction vector and intensities) to solve the linear system of equations
 * and calculate the normal and albedo
 * 
 *  // I1 = p x (s1 · n)
    // I2 = p x (s2 · n)
    // I3  = p x (s3 · n)

    // N = S^-1 * intensities_vector
    // so to calculate N we need to first invert the 3x3 matrix of light directions
    // then multiply that inverted matrix by the intensities vector

    then finally calculate the normalized unit vector n (normal)
    and albedo = magnitude of normal = root(nx^2 + ny^2 + nz^2)
 * 
 */
bool solveLinearSystem(const vector<Vector3D>& light_dirs, const vector<int>& intensities, Vector3D& normal, double& albedo){

    // Create light direction matrix S (3x3)
    double S[3][3];
    for (int i = 0; i < 3; i++) {
        S[i][0] = light_dirs[i].x;
        S[i][1] = light_dirs[i].y;
        S[i][2] = light_dirs[i].z;
    }


    // CALCULATE INVERSE OF S (S^-1) 

    // first calculate the determinant of S
        // review of det (if it's 0 or close to 0, we caan't solve these equations). "how different are our light directions?"
    double det = S[0][0] * (S[1][1] * S[2][2] - S[1][2] * S[2][1])
                - S[0][1] * (S[1][0] * S[2][2] - S[1][2] * S[2][0])
                + S[0][2] * (S[1][0] * S[2][1] - S[1][1] * S[2][0]);
                 

    // Check if matrix is invertible (i.e., if det is not close to 0)
    if (fabs(det) < 1e-6)
    {
        return false;
    }
    
    double invDet = 1.0 / det; // use this to multiply ()
    double S_inv[3][3];
    
    // calc S_inverse (3x3) using determinant
    S_inv[0][0] =  (S[1][1] * S[2][2] - S[1][2] * S[2][1]) * invDet;
    S_inv[0][1] = -(S[0][1] * S[2][2] - S[0][2] * S[2][1]) * invDet;
    S_inv[0][2] =  (S[0][1] * S[1][2] - S[0][2] * S[1][1]) * invDet;
    
    S_inv[1][0] = -(S[1][0] * S[2][2] - S[1][2] * S[2][0]) * invDet;
    S_inv[1][1] =  (S[0][0] * S[2][2] - S[0][2] * S[2][0]) * invDet;
    S_inv[1][2] = -(S[0][0] * S[1][2] - S[0][2] * S[1][0]) * invDet;
    
    S_inv[2][0] =  (S[1][0] * S[2][1] - S[1][1] * S[2][0]) * invDet;
    S_inv[2][1] = -(S[0][0] *S[2][1] - S[0][1] * S[2][0]) * invDet;
    S_inv[2][2] =  (S[0][0] * S[1][1] - S[0][1] * S[1][0]) * invDet;
    
    // calculate Normal by multiplying S^-1 by intensities vector (I)
    // N = S^-1 * I
    normal.x = S_inv[0][0]*intensities[0] + S_inv[0][1]*intensities[1] + S_inv[0][2]*intensities[2];
    normal.y = S_inv[1][0]*intensities[0] + S_inv[1][1]*intensities[1] + S_inv[1][2]*intensities[2];
    normal.z = S_inv[2][0]*intensities[0] + S_inv[2][1]*intensities[1] + S_inv[2][2]*intensities[2];
    
    // Calculate albedo (magnitude of Normal)
    albedo = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
    
    // Normalize the normal vector so its a unit vector
    if (albedo > 0)
    {
        normal.x /= albedo;
        normal.y /= albedo;
        normal.z /= albedo;
    }
    
    return true;
}



// Draw a line representing the normal projection
void drawNormalLine(Image* an_image, int row, int col, const Vector3D& normal){
    // Scale factor for line
    const int scale = 10;
    
    // Project 3D normal onto image plane (dropping z component) and calculate endpoint
    int start_row = row;
    int start_col = col;
    int end_row = row + static_cast<int>(normal.x * scale);
    int end_col = col + static_cast<int>(normal.y * scale);
    


    // Calculate the difference between end points
    float d_row = end_row - start_row;
    float d_col = end_col - start_col;
    
    // steps we need == longest difference
    float steps = std::max(std::abs(d_row), std::abs(d_col));
    
    // calc how much to increment row and col each step
    float row_increment = d_row / steps;
    float col_increment = d_col / steps;
    
    float curr_row = start_row;
    float curr_col = start_col;
    
    // Draw each point along the line
    for (int i = 0; i <= steps; i++){
        // Round to nearest pixel and draw
        int pixel_row = std::round(curr_row);
        int pixel_col = std::round(curr_col);
        
        // Check if point within image bounds
        if (pixel_row >= 0 && pixel_row < an_image->num_rows() && 
            pixel_col >= 0 && pixel_col < an_image->num_columns()){
            an_image->SetPixel(pixel_row, pixel_col, 255);  // White line
        }
        
        // proceed to next point
        curr_row += row_increment;
        curr_col += col_increment;
    }


    // draw black dot at base
    an_image->SetPixel(row,col,0);
    
}




int main(int argc, char **argv){

    if (argc != 9) {
        printf("Usage: %s {input directions} {object image 1} {object image 2} {object image 3} {step} {threshold} {output normals} {output albedo}\n", argv[0]);
        return 0;
    }
    
    const string directions_file(argv[1]);
    const string object_files[] = {argv[2], argv[3], argv[4]};
    const int step = stoi(argv[5]);
    const int threshold = stoi(argv[6]);
    const string normals_file(argv[7]);
    const string albedo_file(argv[8]);

    // Read light directions from s2
    vector<Vector3D> light_dirs = readLightDirections(directions_file);

    // Read object images
    vector<Image> images;

    for (int i = 0; i < 3; i++){
        Image an_image;
        if (!ReadImage(object_files[i], &an_image)){
            cout << "Can't open file " << object_files[i] << endl;
            return 0;
        }

        images.push_back(an_image);
    }

    
    // Create output images
    Image normals_image = images[0]; 
    Image albedo_image = images[0];
    
    // Initialize albedo image to black
    for (int i = 0; i < albedo_image.num_rows(); ++i){
        for (int j = 0; j < albedo_image.num_columns(); ++j){
            albedo_image.SetPixel(i, j, 0);
        }
    }
    
    // Find max albedo for scaling
    double max_albedo = 0;

    
    // compute normals and find max albedo
    vector<vector<Vector3D>> normals(images[0].num_rows(), vector<Vector3D>(images[0].num_columns()));
    vector<vector<double>> albedos(images[0].num_rows(), vector<double>(images[0].num_columns()));

    for (int x = 0; x < images[0].num_rows(); ++x){
        for (int y = 0; y < images[0].num_columns(); ++y){
            if (isPixelVisible(x, y, images, threshold)) {
                // Get intensities for this pixel from all images
                vector<int> intensities;
                for (const auto& an_image : images) {
                    intensities.push_back(an_image.GetPixel(x, y));
                }
                
                Vector3D normal;
                double albedo;
                
                if (solveLinearSystem(light_dirs, intensities, normal, albedo)) {
                    normals[x][y] = normal;
                    albedos[x][y] = albedo;
                    max_albedo = max(max_albedo, albedo);
                }
            }
        }
    }
    
    // create output images
    for (int x = 0; x < images[0].num_rows(); ++x){
        for (int y = 0; y < images[0].num_columns(); ++y){

            // Draw normal lines at grid points
            if (x % step == 0 && y % step == 0 && isPixelVisible(x, y, images, threshold))
            {
                drawNormalLine(&normals_image, x, y, normals[x][y]);
            }
            
            // Scale and set albedo
            if (isPixelVisible(x, y, images, threshold))
            {
                // make int! 
                int scaled_albedo = static_cast<int>((albedos[x][y] / max_albedo) * 255);
                albedo_image.SetPixel(x, y, scaled_albedo);
            }
        }
    }
    
    // output images
    if (!WriteImage(normals_file, normals_image)){
        cout << "Can't write to file " << normals_file << endl;
        return 0;
    }
    if (!WriteImage(albedo_file, albedo_image)){
        cout << "Can't write to file " << albedo_file << endl;
        return 0;
    }
    
    
    return 0;
}