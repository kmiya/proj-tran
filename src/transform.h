#ifndef PROJ_TRANSFORM_H_
#define PROJ_TRANSFORM_H_

#include <vector>
#include <opencv2/core/core.hpp>

namespace proj_transform {

class Transform {
 public:
   Transform(const cv::Mat& inputImage_, cv::Mat* outputImage_);
   ~Transform();
   // Perspective Transform a image from src to dst
   void setInputPoints(const std::vector<cv::Point3d>& src);
   void myPerspectiveTransform();

 private:
  cv::Mat inputImage;
  cv::Mat outputImage;
  std::vector<cv::Point3d> inputPoints;
  std::vector<cv::Point3d> outputPoints;

  // Transform image by the given 3*3 matrix
  void transformByMatrix(cv::Mat& in, const cv::Mat& h, cv::Mat* out);
  // Print cv::Mat elements.
  void show(const cv::Mat& x);
  // Return a solution of the system of linear equations.
  void solveLinearEquations (const cv::Mat& left, const cv::Mat& right, cv::Mat* result);
  // Exchange A-th row for B-th row
  void exchangeRow(const int from, const int to, cv::Mat *m);

};

} // namespace proj_transform

#endif // PROJ_TRANSFORM_H_
