#include "Image.hpp"
#include "MyROI.hpp"

using namespace cv;

MyROI::MyROI() {
  upper_corner = Point(0, 0);
  lower_corner = Point(0, 0);
}

MyROI::MyROI(Point u_corner, Point l_corner, Mat src) {
  upper_corner = u_corner;
  lower_corner = l_corner;

  color = Scalar(48, 48, 255);
  border_thickness = 2;
  roi_cropped = src(Rect(
        u_corner.x, u_corner.y,
        l_corner.x-u_corner.x, l_corner.y-u_corner.y));
}

void MyROI::draw_rectangle(Mat src) {
  rectangle(src, upper_corner, lower_corner, color, border_thickness);
}
