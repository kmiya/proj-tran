#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include "main.h"

using namespace std;
using namespace cv;
using namespace proj;
using namespace proj_transform;

namespace {

Mat tmpImage;
Transform *img;

} // namespace

int main(int argc, char **argv)
{
  // Read image given by user
  if (argc < 2) {
    usage();
    exit(EXIT_FAILURE);
  }
  int width = 420, height = 594;
  if (argc == 4) {
    width  = atoi(argv[2]);
    height = atoi(argv[3]);
    cout << width << ", " << height << endl;
  }
  const Mat inputImage = imread(argv[1]);
  inputImage.copyTo(tmpImage);

  // initialize output image
  Mat outputImage = Mat::zeros( Size(width, height), inputImage.type() );
  // create instance for image transformation
  img = new Transform(inputImage, &outputImage);
  // create a window and register a callback function for mouse events
  namedWindow("Input image", 0);
  setMouseCallback("Input image", onMouse);
  imshow("Input image", tmpImage);

  cout << "\nSelect corners clockwise from the upper left.\n";
  cout << "Press any key to exit." << endl;
  if (waitKey()) {
    return 1;
  }
}
namespace proj {

// **********************************************************
// proj::usage()
// ----------------------------------------------------------
// Usage: proj input_image [width] [height]
//
void usage() {
  cout << "Usage:\tproj input_image [output_width] [output_height]" << endl;
}
// **********************************************************
// proj::onMouse()
// ----------------------------------------------------------
// Draw circle when mouse will clicked.
// If there are 4 circles, then transformation will start.
//
void onMouse (int event, int x, int y, int flags, void *param = NULL)
{
  static unsigned int edge = 0;
  static vector<Point3d> inputPoints;

  // get mouse events
  switch (event) {
  case CV_EVENT_LBUTTONDOWN:
    inputPoints.push_back(Point3d(x, y, 1));
    edge++;
    break;
  default:
    return;
  }
  // draw a circle in mouse coord.
  Point center(x, y);
  const unsigned int radius = 20;
  const unsigned int thickness = 5;
  circle(tmpImage, center, radius, CV_RGB(255, 255, 255), thickness);
  imshow("Input image", tmpImage);
  if (edge == 4) {
    img->setInputPoints(inputPoints);
    img->myPerspectiveTransform();
  }
}

} // namespace proj
