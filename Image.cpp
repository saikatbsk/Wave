#include <iostream>
#include "Image.hpp"

using namespace cv;
using namespace std;

Image::Image() {
  //-- nothing
}

Image::Image(int webCamera) {
  cameraIndex = webCamera;
  capture = VideoCapture(webCamera);

  if(capture.isOpened() == false) {
    cout << "Error opening camera!" << endl;
  }
}
