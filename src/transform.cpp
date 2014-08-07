#include <iostream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>

#include "transform.h"

using namespace std;
using namespace cv;

namespace proj_transform {

// **********************************************************
// proj_transform::Transform::Transform()
// ----------------------------------------------------------
//
Transform::Transform(const Mat& inputImage_, Mat* outputImage_) {
  inputImage  = inputImage_;
  outputImage = *outputImage_;
  outputPoints.push_back( Point3d(0, 0, 1) );
  outputPoints.push_back( Point3d(outputImage.cols, 0, 1) );
  outputPoints.push_back( Point3d(outputImage.cols, outputImage.rows, 1) );
  outputPoints.push_back( Point3d(0, outputImage.rows, 1) );
}
void Transform::setInputPoints(const std::vector<cv::Point3d>& src) {
  inputPoints = src;
}
// **********************************************************
// proj_transform::Transform::perspectiveTransform()
// ----------------------------------------------------------
// calculate homography matrix and use it to transform image
//
void Transform::myPerspectiveTransform() {
  const vector<Point3d> src = inputPoints;
  const vector<Point3d> dst = outputPoints;
  Mat A(8, 8, CV_64F, Scalar::all(0));
  for (unsigned int i = 0; i < src.size(); ++i) {
    A.at<double>(i*2, 0) = src[i].x;
    A.at<double>(i*2, 1) = src[i].y;
    A.at<double>(i*2, 2) = 1;
    A.at<double>(i*2, 3) = 0;
    A.at<double>(i*2, 4) = 0;
    A.at<double>(i*2, 5) = 0;
    A.at<double>(i*2, 6) = -dst[i].x * src[i].x;
    A.at<double>(i*2, 7) = -dst[i].x * src[i].y;
    A.at<double>(i*2+1, 0) = 0;
    A.at<double>(i*2+1, 1) = 0;
    A.at<double>(i*2+1, 2) = 0;
    A.at<double>(i*2+1, 3) = src[i].x;
    A.at<double>(i*2+1, 4) = src[i].y;
    A.at<double>(i*2+1, 5) = 1;
    A.at<double>(i*2+1, 6) = -dst[i].y * src[i].x;
    A.at<double>(i*2+1, 7) = -dst[i].y * src[i].y;
  }
  // solve the system of linear equations
  Mat solution, right(8, 1, CV_64F);
  for (unsigned int i = 0; i < dst.size(); ++i) {
    right.at<double>(2*i, 0)   = dst[i].x;
    right.at<double>(2*i+1, 0) = dst[i].y;
  }
  solveLinearEquations(A, right, &solution);

  // copy solutions to homography matrix
  Mat h(3, 3, CV_64F, Scalar::all(0)); // homography matrix
  for (unsigned int y = 0; y < 3; ++y) {
    for (unsigned int x = 0; x < 3; ++x) {
      if (y == 2 && x == 2) {
        h.at<double>(y, x) = 1.;
      } else {
        h.at<double>(y, x) = solution.at<double>(3*y+x, solution.cols-1);
      }
    }
  }
  // normalize homography matrix
  h /= h.at<double>(2, 2);
  cout << "\n*** Homography matrix ***" << endl;
  show(h);
  Mat in  = inputImage;
  Mat out = outputImage;
  // transform image using homography matrix
  transformByMatrix(in, h, &out);
  namedWindow("Output image", 1);
  imshow("Output image", outputImage);
}
// **********************************************************
// proj_transform::Transform::transformByMatrix()
// ----------------------------------------------------------
// vector product 3*3 transform matrix and image
//
void Transform::transformByMatrix(Mat& in, const Mat& h, Mat* out) {
  for(int y = 0; y < in.rows; ++y) {
    uchar *p = in.ptr(y);
    for(int x = 0; x < in.cols; ++x) {
      const int ch = in.channels();
      double xs_ = h.at<double>(0, 0) * x + h.at<double>(0, 1) * y + h.at<double>(0, 2);
      double ys_ = h.at<double>(1, 0) * x + h.at<double>(1, 1) * y + h.at<double>(1, 2);
      double s   = h.at<double>(2, 0) * x + h.at<double>(2, 1) * y + h.at<double>(2, 2);
      int xs = static_cast<int>(xs_ / s);
      int ys = static_cast<int>(ys_ / s);
      if (xs >= out->cols || ys >= out->rows || xs < 0 || ys < 0) {
        continue;
      } else {
        uchar *q = out->ptr(ys);
        q[xs*out->channels()+0] = p[x*ch+0];
        q[xs*out->channels()+1] = p[x*ch+1];
        q[xs*out->channels()+2] = p[x*ch+2];
      }
    }
  }
}
// **********************************************************
// proj_transform::Transform::show()
// ----------------------------------------------------------
// print cv::Mat elements.
//
void Transform::show(const Mat& m) {
  cout.setf(ios::fixed, ios::floatfield);
  for (int y = 0; y < m.rows; ++y) {
    for (int x = 0; x < m.cols; ++x) {
      cout << setprecision(3) << " " << m.at<double>(y, x);
    }
    cout << endl;
  }
  cout << endl;
}
// **********************************************************
// proj_transform::Transform::solveLinearEquations()
// ----------------------------------------------------------
// solve the system of linear equations using Gauss-Jordan elimination
//
void Transform::solveLinearEquations (const Mat& left, const Mat& right, Mat* result)
{
  // error check
  if (left.cols != right.rows || left.rows != right.rows || right.cols != 1)
    exit(EXIT_FAILURE);
  // create coefficient matrix
  Mat m(left.rows, left.cols+1, CV_64F);
  Mat roi = Mat(m, Rect(Point(0, 0), left.size()));
  left.copyTo(roi);
  right.copyTo(m.col(m.cols-1));
  double factor = 1;
  // iteration of Gauss-Jordan elimination
  for (int x = 0, y = 0; y < m.rows; ++x, ++y) {
    // forward elimination
    if (m.at<double>(y, x) == 0.) {
      int noNums = 1;
      for (int n = y+1; n < m.rows; ++n) {
        if (m.at<double>(n, x) != 0.) {
          exchangeRow(n, y, &m);
          noNums = 0;
          break;
        }
      }
      if (noNums) exit(EXIT_FAILURE); // there are no pivots
    }
    // backward substitution
    factor = m.at<double>(y, x);
    m.row(y) /= factor;
    for (int n = 0; n < m.rows; ++n) {
      if (n != y) {
        m.row(n) -= m.row(y) * m.at<double>(n, x);
      }
    }
  }
  *result = m.col(m.cols-1);
}
// **********************************************************
// proj_transform::Transform::show()
// ----------------------------------------------------------
void Transform::exchangeRow(const int from, const int to, Mat *m) {
  // data copy
  // see Mat::row in OpenCV v2.4 documentation
  // http://docs.opencv.org/modules/core/doc/basic_structures.html#mat-row
  Mat tmp;
  m->row(to).copyTo(tmp);
  m->row(from).copyTo(m->row(to));
  tmp.copyTo(m->row(from));
}

} // namespace proj_transform
