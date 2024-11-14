/**
 * Daniel Kaijzer
 * 
 * Calculates position and radius of circle
 * This is accomplished by converting the input image to a binary image
 * then calculate the area of binary object, then centroid 
 * radius = widest part of circle / 2
 */

#include <iostream>
#include <string>
#include <fstream>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

void ConvertToBinaryImage(Image *an_image, int T){
    if (an_image == nullptr) abort();

    // iterate through pixels
    for (int i = 0; i < an_image->num_rows(); ++i){
        for (int j = 0; j < an_image->num_columns(); ++j){

            // make binary, white or black, depending on threshold
            if (an_image->GetPixel(i,j) > T){
                an_image->SetPixel(i,j,255); // make white
            }
            else{
                an_image->SetPixel(i,j,0); // make black
            }
        }
    }
}

void writeOutputFile(const int xbar, const int ybar, const int radius, const string output_file){
    ofstream ofs(output_file);
    ofs << to_string(xbar) << " " << to_string(ybar) << " " << radius;
    ofs.close();

}

void calculateGeometry(Image *binary_image, const string output_file){
    if (binary_image == nullptr) abort();

    int rows = binary_image->num_rows();
    int cols = binary_image->num_columns();

    int leftmost = 1000000;
    int rightmost = -1;
    int sum_x = 0;
    int sum_y = 0;
    int area = 0;

    // iterate through image
    for (int x = 0; x < rows; ++x){
        for (int y = 0; y < cols; ++y){

            int cur = binary_image->GetPixel(x,y);

            if(cur == 0){continue;} // skip background

            else{
                // iterate area
                area++;
                sum_x += x;
                sum_y += y;

                // check if pixel is leftmost
                if (y < leftmost){
                    leftmost = y;
                }
                // check if pixel is rightmost
                if (y > rightmost){
                    rightmost = y;
                }
            }
        }
    }
    
    int xbar = sum_x / area;
    int ybar = sum_y / area;
    int radius = (rightmost-leftmost)/2;

    writeOutputFile(xbar, ybar,radius, output_file);

}

int main(int argc, char **argv){
  
  if (argc!=4) {
    printf("Usage: %s {input gray–level sphere image} {input threhsold value} {output parameters file} \n", argv[0]);
    return 0;
  }
  const string input_file(argv[1]);
  const int T = stoi(argv[2]);
  const string output_file(argv[3]);


  Image an_image; // declare image object (2D array)
  if (!ReadImage(input_file, &an_image)) {
    cout <<"Can't open file " << input_file << endl;
    return 0;
  }

  Image binary = an_image;
  ConvertToBinaryImage(&binary, T);
  calculateGeometry(&binary, output_file);
  
  if (!WriteImage("binary.pgm", binary)){
    cout << "Can't write to file " << "binary.pgm" << endl;
    return 0;
  }
}

