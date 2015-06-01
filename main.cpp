#include "Image.hpp"
#include "MyROI.hpp"
#include "Hand.hpp"
#include <getopt.h>
#include <opencv2/opencv.hpp>

#define SAMPLES 12

using namespace cv;

//-- global variables
int camera;
int gesture;
int rectLen;
int avg_color[SAMPLES][3];
int l_bound[SAMPLES][3];
int u_bound[SAMPLES][3];
int c_lower[SAMPLES][3];
int c_upper[SAMPLES][3];
vector<MyROI> roi;
Point positionCentre;

//-- ***

void init() {
    camera = 0;
    gesture = 0;
    rectLen = 12;
}

void printText(Mat src, string text) {
    int fontFace = FONT_HERSHEY_PLAIN;

    putText(
            src, text,
            Point(src.cols/12 + 2, src.rows/12 + 2),
            fontFace, 1.2f, Scalar(0, 0, 0), 2);
    putText(
            src, text,
            Point(src.cols/12, src.rows/12),
            fontFace, 1.2f, Scalar(254, 254, 254), 1);
}

void waitForPalm(Image *im) {
    im->capture >> im->src;
    flip(im->src, im->src, 1);

    //-- add elements to the bottom
    roi.push_back(MyROI(
                Point(im->src.cols/3, im->src.rows/6),
                Point(im->src.cols/3 + rectLen, im->src.rows/6 + rectLen),
                im->src)); //-- sample 1
    roi.push_back(MyROI(
                Point(im->src.cols/2.5, im->src.rows/5),
                Point(im->src.cols/2.5 + rectLen, im->src.rows/5 + rectLen),
                im->src)); //-- sample 2
    roi.push_back(MyROI(
                Point(im->src.cols/2, im->src.rows/3),
                Point(im->src.cols/2 + rectLen, im->src.rows/3 + rectLen),
                im->src)); //-- sample 3
    roi.push_back(MyROI(
                Point(im->src.cols/3, im->src.rows/2),
                Point(im->src.cols/3 + rectLen, im->src.rows/2 + rectLen),
                im->src)); //-- sample 4
    roi.push_back(MyROI(
                Point(im->src.cols/2, im->src.rows/2),
                Point(im->src.cols/2 + rectLen, im->src.rows/2 + rectLen),
                im->src)); //-- sample 5
    roi.push_back(MyROI(
                Point(im->src.cols/1.8, im->src.rows/1.5),
                Point(im->src.cols/1.8 + rectLen, im->src.rows/1.5 + rectLen),
                im->src)); //-- sample 6
    roi.push_back(MyROI(
                Point(im->src.cols/3, im->src.rows/1.3),
                Point(im->src.cols/3 + rectLen, im->src.rows/1.3 + rectLen),
                im->src)); //-- sample 7
    roi.push_back(MyROI(
                Point(im->src.cols/2.5, im->src.rows/1.5),
                Point(im->src.cols/2.5 + rectLen, im->src.rows/1.5 + rectLen),
                im->src)); //-- sample 8
    roi.push_back(MyROI(
                Point(im->src.cols/2.5, im->src.rows/1.7),
                Point(im->src.cols/2.5 + rectLen, im->src.rows/1.7 + rectLen),
                im->src)); //-- sample 9
    roi.push_back(MyROI(
                Point(im->src.cols/2.2, im->src.rows/1.7),
                Point(im->src.cols/2.2 + rectLen, im->src.rows/1.7 + rectLen),
                im->src)); //-- sample 10
    roi.push_back(MyROI(
                Point(im->src.cols/2.2, im->src.rows/1.5),
                Point(im->src.cols/2.2 + rectLen, im->src.rows/1.5 + rectLen),
                im->src)); //-- sample 11
    roi.push_back(MyROI(
                Point(im->src.cols/3.4, im->src.rows/1.7),
                Point(im->src.cols/3.4 + rectLen, im->src.rows/1.7 + rectLen),
                im->src)); //-- sample 12

    /*
     * loop for 50 frames, draw rectangles and ask
     * user to cover the rectangles with palm
     */
    for(int i=0 ; i<50 ; i++) {
        im->capture >> im->src;
        flip(im->src, im->src, 1);

        for(int j=0 ; j<SAMPLES ; j++) {
            roi[j].draw_rectangle(im->src);
        }

        string text = string("Cover rectangles with palm");
        printText(im->src, text);

        imshow("Wave", im->src);

        if(waitKey(1) >= 0) break;
    }
}

int median(vector<int> value) {
    int med;
    size_t size = value.size();

    sort(value.begin(), value.end());

    if(size % 2 == 0) {
        med = value[size / 2-1];
    }
    else {
        med = value[size / 2];
    }

    return med;
}

void avgColor(Image *im, MyROI roi, int avg[3]) {
    Mat img;
    roi.roi_cropped.copyTo(img);

    vector<int> medHue;
    vector<int> medSat;
    vector<int> medVal;

    for(int i=2 ; i<img.rows-2 ; i++) {
        for(int j=2 ; j<img.cols-2 ; j++) {
            medHue.push_back(img.data[img.channels() * (img.cols*i + j) + 0]);
            medSat.push_back(img.data[img.channels() * (img.cols*i + j) + 1]);
            medVal.push_back(img.data[img.channels() * (img.cols*i + j) + 2]);
        }
    }

    avg[0] = median(medHue);
    avg[1] = median(medSat);
    avg[2] = median(medVal);
}

void getAvgColor(Image *im) {
    im->capture >> im->src;
    flip(im->src, im->src, 1);

    for(int i=0 ; i<30 ; i++) {
        im->capture >> im->src;
        flip(im->src, im->src, 1);

        cvtColor(im->src, im->src, CV_BGR2HSV);

        for(int j=0 ; j<SAMPLES ; j++) {
            avgColor(im, roi[j], avg_color[j]);
            roi[j].draw_rectangle(im->src);
        }

        cvtColor(im->src, im->src, CV_HSV2BGR);

        string text = string("Finding average color");
        printText(im->src, text);

        imshow("Wave", im->src);

        if(waitKey(1) >= 0)
            break;
    }
}

void initBounds() {
    for(int i=0 ; i < SAMPLES ; i++) {
        l_bound[i][0] = 12;
        u_bound[i][0] = 7;

        l_bound[i][1] = 30;
        u_bound[i][1] = 40;

        l_bound[i][2] = 80;
        u_bound[i][2] = 80;
    }
}

void normalizeColors() {
    for(int i=0 ; i < SAMPLES ; i++) {
        for(int j=0 ; j < 3 ; j++) {
            c_lower[i][j] = l_bound[i][j];
            c_upper[i][j] = u_bound[i][j];
        }
    }

    for(int i=0 ; i < SAMPLES ; i++) {
        for(int j=0 ; j < 3 ; j++) {
            if(avg_color[i][j]-l_bound[i][j] < 0) {
                c_lower[i][j] = avg_color[i][j];
            }
            if(avg_color[i][j]+u_bound[i][j] > 255) {
                c_upper[i][j] = 255-avg_color[i][j];
            }
        }
    }
}

void binary(Image *im) {
    Scalar lBound;
    Scalar uBound;

    for(int i=0 ; i<SAMPLES ; i++) {
        normalizeColors();

        lBound = Scalar(
                avg_color[i][0]-c_lower[i][0],
                avg_color[i][1]-c_lower[i][1],
                avg_color[i][2]-c_lower[i][2]);
        uBound = Scalar(
                avg_color[i][0]+c_upper[i][0],
                avg_color[i][1]+c_upper[i][1],
                avg_color[i][2]+c_upper[i][2]);

        im->binList.push_back(Mat(im->srcL.rows, im->srcL.cols, CV_8U));
        inRange(im->srcL, lBound, uBound, im->binList[i]);
    }

    im->binList[0].copyTo(im->bin);

    for(int i=1 ; i<SAMPLES ; i++) {
        im->bin += im->binList[i];
    }

    medianBlur(im->bin, im->bin, 7);
}

int biggestContour(vector<vector<Point> > contours) {
    int index = -1;
    int size = 0;

    for(int i=0 ; i<contours.size() ; i++) {
        if(contours[i].size() > size) {
            size = contours[i].size();
            index = i;
        }
    }

    return index;
}

void makeContours(Image *im, Hand *hand) {
    Mat bw;

    im->bin.copyTo(bw);
    findContours(
            bw,
            hand->contours,
            CV_RETR_EXTERNAL,
            CV_CHAIN_APPROX_NONE);

    hand->initVectors();
    hand->contourIDx = biggestContour(hand->contours);

    if(hand->contourIDx != -1) {
        hand->boundRect = boundingRect(Mat(hand->contours[hand->contourIDx]));

        convexHull(
                Mat(hand->contours[hand->contourIDx]),
                hand->hullP[hand->contourIDx],
                false, true);
        convexHull(
                Mat(hand->contours[hand->contourIDx]),
                hand->hullI[hand->contourIDx],
                false, false);
        approxPolyDP(
                Mat(hand->contours[hand->contourIDx]),
                hand->hullP[hand->contourIDx],
                18, true);

        if(hand->contours[hand->contourIDx].size() > 3) {
            convexityDefects(
                    hand->contours[hand->contourIDx],
                    hand->hullI[hand->contourIDx],
                    hand->defects[hand->contourIDx]);

            hand->eleminateDefects(im);
        }

        bool isHand = hand->detectIfHand();
        if(isHand) {
            /*
             * uncomment to find the centroid
             */
            // positionCentre = hand->findCentroid(im, true);

            /*
             * uncomment to draw minimum enclosing circle
             */
            // hand->drawMinEnclosingCircle(im);

            /*
             * uncomment to draw fingers
             */
            // hand->drawFingers(im);

            hand->drawHand(im);
            hand->getFingerTips(im);
            hand->drawFingerTips(im);
        }
    }
}

int main(int argc, char **argv) {
    int opt;
    int count = 0;

    Point prePosition;
    Point curPosition;

    init();

    while((opt = getopt(argc, argv, "vc:")) != -1) {
        switch(opt) {
            case 'v':
                cout << "Version 1.0" << endl;
                exit(0);
            case 'c':
                camera = atoi(optarg);
        }
    }

    Image im(camera);
    Hand hand;

    //-- capture an image
    im.capture >> im.src;

    //-- creating a black image the same size as the source
    im.canvas = Mat::zeros(im.src.size(), CV_8UC3);

    //-- because mirror images are awfully scary
    flip(im.src, im.src, 1);

    //-- creating gui object(s)
    namedWindow("Wave", CV_WINDOW_AUTOSIZE);

    /*
     * wait for palm cover to get
     * the average color of hand
     */
    waitForPalm(&im);
    getAvgColor(&im);
    initBounds();

    //-- time warp.. Engage
    while(1) {
        hand.frameNumber++;
        im.capture >> im.src;
        flip(im.src, im.src, 1);

        //-- creating clone images
        im.src.copyTo(im.srcRAW);
        im.src.copyTo(im.srcL);

        for(int i=0 ; i<3 ; i++) {
            pyrDown(im.srcL, im.srcL);
            pyrUp(im.srcL, im.srcL);
        }

        blur(im.srcL, im.srcL, Size(3, 3));
        cvtColor(im.srcL, im.srcL, CV_BGR2HSV);

        //-- create binary image
        binary(&im);

        //-- make contours
        makeContours(&im, &hand);

        //-- tracking 0th finger
        if(hand.nrFinger > 0) {
            if(count < 1) {
                prePosition = hand.fingerTips[0];
                curPosition = hand.fingerTips[0];

                count++;
            }
            else {
                Point temp = curPosition;
                curPosition = hand.fingerTips[0];
                prePosition = temp;
            }
        }

        //-- finger drawing
        if(hand.nrFinger == 2 && gesture == 0)
            gesture = 1;

        if(gesture == 1) {
            if(hand.nrFinger == 1) {
                //-- process involving finger drawing
                line(im.canvas, prePosition, curPosition, Scalar(60, 60, 160), 4);
            }
            else if(hand.nrFinger >= 4) {
                gesture = 0;
                im.canvas = Mat::zeros(im.src.size(), CV_8UC3);
            }
        }

        //-- convert back to BGR
        cvtColor(im.srcL, im.srcL, CV_HSV2BGR);

        pyrDown(im.bin, im.bin);
        pyrDown(im.bin, im.bin);

        Rect roi_bin(Point(3*im.src.cols/4, 0), im.bin.size());
        vector<Mat> chnls;
        Mat res;

        for(int i=0 ; i<3 ; i++) {
            chnls.push_back(im.bin);
        }

        merge(chnls, res);
        res.copyTo(im.src(roi_bin));

        im.src = im.src+im.canvas;
        imshow("Wave", im.src);

        //-- break time warp
        if(waitKey(1) == 27) break;
    }

    destroyAllWindows(); //-- umm..ya @_@ ok
    im.capture.release();
}
