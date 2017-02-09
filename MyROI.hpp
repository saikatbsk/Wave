#ifndef _MYROI_HPP_
#define _MYROI_HPP_

#include <opencv2/opencv.hpp>

using namespace cv;

class MyROI {
	public:
		int border_thickness;
		Point upper_corner, lower_corner;
		Scalar color;
		Mat roi_cropped;

		MyROI(Point u_corner, Point l_corner, Mat src);
		MyROI();

		void draw_rectangle(Mat src);
};

#endif
