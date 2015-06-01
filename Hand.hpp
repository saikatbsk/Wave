#ifndef _HAND_HPP_
#define _HAND_HPP_

#include <string>
#include "Image.hpp"

#define PI 3.14159

using namespace cv;
using namespace std;

class Hand {
    public:
        int frameNumber;
        int contourIDx;
        int nrDefects;
        int nrFinger; //-- number of fingers
        Rect boundRect;
        vector<vector<Point> > contours;
        vector<vector<int> > hullI;
        vector<vector<Point> > hullP;
        vector<vector<Vec4i> > defects;
        vector<Point> fingerTips;
        double bRect_w;
        double bRect_h;
        bool isHand;

        Hand();

        void initVectors();
        void eleminateDefects(Image *im);
        bool detectIfHand();
        void getFingerTips(Image *im);
        Point findCentroid(Image *im, bool draw);
        void drawMinEnclosingCircle(Image *im);
        void drawFingerTips(Image *im);
        void drawFingers(Image *im);
        void drawHand(Image *im);
        float getAngle(Point s, Point f, Point e);

    private:
        int fontFace;

        float distanceP2P(Point a, Point b);
        void remRedundantEnd(Vector<Vec4i> newDefects, Image *im);
        void analyzeContours();
        int checkForOneFinger(Image *im);
        string int2String(int num);
};

#endif
