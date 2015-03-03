#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <opencv2/opencv.hpp>

using namespace cv;

class Image {
  public:
    int cameraIndex;
    Mat src; //-- used for the source image
    Mat bin; //-- used for the binary image
    Mat srcRAW; //-- clone image
    Mat srcL; //-- low resolution image
    Mat canvas; //-- canvas to draw on
    vector<Mat> binList;
    VideoCapture capture;

    Image(int webCamera);
    Image();
};

#endif
