/**
 * Compute normal vector to the spheres surface at a given point
 * knowing the points cooridnates (in image coordinate frame)
 * and coordinates of the center and redius of spheres projection onto the image plane 
 * (assume orthonormal projection)
 * 
 */

#include <iostream>
#include <string>
#include <fstream>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

void computeNormal(){
    // TO DO
}

int main(int argc, char **argv){
  
  if (argc!=4) {
    printf("Usage: %s {input parameters filename} {input sphere image 1 filename} {input sphere image 2 filename} {input sphere image 3 filename} {output directions filename} \n", argv[0]);
    return 0;
  }
  const string input_parameters_filename(argv[1]);
  const string input_img1(argv[2]);
  const string input_img2(argv[3]);
  const string input_img3(argv[4]);
  const string output_directions_filename(argv[5]);


  Image image1; // declare image object (2D array)
  if (!ReadImage(input_img1, &image1)) {
    cout <<"Can't open file " << input_img1 << endl;
    return 0;
  }

  Image image2; // declare image object (2D array)
  if (!ReadImage(input_img2, &image2)) {
    cout <<"Can't open file " << input_img2 << endl;
    return 0;
  }

  Image image3; // declare image object (2D array)
  if (!ReadImage(input_img3, &image3)) {
    cout <<"Can't open file " << input_img3 << endl;
    return 0;
  }

  
//   if (!WriteImage("binary.pgm", binary)){
//     cout << "Can't write to file " << "binary.pgm" << endl;
//     return 0;
//   }
}

