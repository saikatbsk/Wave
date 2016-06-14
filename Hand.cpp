#include "Hand.hpp"

using namespace cv;

Hand::Hand() {
    frameNumber = 0;
    nrFinger = 0;
    fontFace = FONT_HERSHEY_PLAIN;
}

void Hand::initVectors() {
    hullI = vector<vector<int> > (contours.size());
    hullP = vector<vector<Point> > (contours.size());
    defects = vector<vector<Vec4i> > (contours.size());
}

string Hand::int2String(int num) {
    stringstream s;
    s << num;
    string str = s.str();

    return str;
}

Point Hand::findCentroid(Image *im, bool draw) {
    Point pt;
    Point centroid;
    int centroid_x;
    int centroid_y;
    int j; //-- number of hull points

    vector<Point>::iterator d = contours[contourIDx].begin();

    while(d != contours[contourIDx].end()) {
        Point v = (*d);

        centroid_x += v.x;
        centroid_y += v.y;

        d++;
        j++;
    }

    centroid_x = centroid_x/j;
    centroid_y = centroid_y/j;

    centroid.x = centroid_x;
    centroid.y = centroid_y;

    if(draw == true) {
        circle(im->src, centroid, 4, Scalar(100, 100, 245), 4);
    }

    return centroid;
}

void Hand::drawMinEnclosingCircle(Image *im) {
    vector<Vec4i>::iterator d = defects[contourIDx].begin();
    vector<Point> points;
    Point2f centre;
    float radius = 0;

    while(d != defects[contourIDx].end()) {
        Vec4i &v = (*d);

        int farIDx = v[2];
        Point pt(contours[contourIDx][farIDx]);

        points.push_back(pt);

        d++;
    }

    if(points.size() > 1) {
        minEnclosingCircle(Mat(points), centre, radius);
        circle(im->src, centre, radius, Scalar(60, 160, 160), 2);
    }
}

void Hand::drawFingerTips(Image *im) {
    Point pt;

    for(int i=0 ; i<fingerTips.size() ; i++) {
        pt = fingerTips[i];

        putText(
                im->src, int2String(i),
                pt-Point(0, 30), fontFace,
                1.2f,Scalar(20, 20, 180), 2);

        circle(im->src, pt, 5, Scalar(20, 20, 245), 4);
        circle(im->src, pt, 10, Scalar(20, 20, 245), 2);
        circle(im->src, pt, 15, Scalar(20, 20, 245), 1);
    }
}

void Hand::drawFingers(Image *im) {
    vector<Vec4i>::iterator d = defects[contourIDx].begin();

    while(d != defects[contourIDx].end()) {
        Vec4i &v = (*d);

        int startIDx = v[0];
        Point ptStart(contours[contourIDx][startIDx]);

        int endIDx = v[1];
        Point ptEnd(contours[contourIDx][endIDx]);

        int farIDx = v[2];
        Point ptFar(contours[contourIDx][farIDx]);

        line(im->src, ptStart, ptFar, Scalar(60, 160, 160), 2);
        line(im->src, ptEnd, ptFar, Scalar(60, 160, 160), 2);
        circle(im->src, ptFar, 3, Scalar(30, 180, 180), 2);
        circle(im->src, ptFar, 9, Scalar(30, 180, 180), 1);

        d++;
    }
}

void Hand::drawHand(Image *im) {
    vector<Point>::iterator d = contours[contourIDx].begin();

    while(d != contours[contourIDx].end()) {
        Point v = (*d);

        circle(im->src, v, 2, Scalar(30, 180, 180), 2);

        d++;
    }
}

int Hand::checkForOneFinger(Image *im) {
    int yTol = boundRect.height/6;
    int dTol = boundRect.width/12;
    Point highestPoint;

    highestPoint.y = im->src.rows;
    vector<Point>::iterator d = contours[contourIDx].begin();

    while(d != contours[contourIDx].end()) {
        Point v = (*d);

        if(v.y < highestPoint.y) {
            highestPoint = v;
        }

        d++;
    }

    int n = 0;

    d = hullP[contourIDx].begin();
    while(d != hullP[contourIDx].end()) {
        Point v = (*d);

        if(
                v.y < highestPoint.y+yTol &&
                abs(v.y-highestPoint.y) > dTol &&
                abs(v.x-highestPoint.x) > dTol) {
            n++;
        }

        d++;
    }

    if(n == 0) {
        fingerTips.push_back(highestPoint);
        return 1;
    }

    return 0;
}

void Hand::getFingerTips(Image *im) {
    fingerTips.clear();

    int i = 0;
    vector<Vec4i>::iterator d = defects[contourIDx].begin();

    while(d != defects[contourIDx].end()) {
        Vec4i &v = (*d);

        int startIDx = v[0];
        Point ptStart(contours[contourIDx][startIDx]);

        int endIDx = v[1];
        Point ptEnd(contours[contourIDx][endIDx]);

        int farIDx = v[2];
        Point ptFar(contours[contourIDx][farIDx]);

        if(i == 0) {
            fingerTips.push_back(ptStart);
            i++;
        }

        fingerTips.push_back(ptEnd);
        d++;
        i++;
    }

    if(fingerTips.size() == 0) {
        if(checkForOneFinger(im)) {
            nrFinger = 1;
        }
        else {
            nrFinger = 0;
        }
    }
    else {
        nrFinger = fingerTips.size();
    }
}

void Hand::analyzeContours() {
    bRect_h = boundRect.height;
    bRect_w = boundRect.width;
}

bool Hand::detectIfHand() {
    analyzeContours();
    double h = bRect_h;
    double w = bRect_w;

    isHand = true;

    if(fingerTips.size() > 5) {
        isHand = false;
    }
    else if(h == 0 || w == 0) {
        isHand = false;
    }
    else if(h/w > 4 || w/h > 4) {
        isHand = false;
    }
    else if(boundRect.x < 20) {
        isHand = false;
    }

    return isHand;
}

float Hand::distanceP2P(Point a, Point b) {
    float d = sqrt(fabs(pow(a.x-b.x, 2) + pow(a.y-b.y, 2)));

    return d;
}

float Hand::getAngle(Point s, Point f, Point e) {
    float l1 = distanceP2P(f, s);
    float l2 = distanceP2P(f, e);
    float dot = (s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
    float angle = acos(dot / (l1*l2));

    angle = angle*180 / PI;

    return angle;
}

void Hand::remRedundantEnd(vector<Vec4i> newDefects, Image *im) {
    Vec4i temp;
    float tolerance = bRect_w/5;
    int startIDx, endIDx;
    int startIDx2, endIDx2;

    for(int i=0 ; i<newDefects.size() ; i++) {
        for(int j=i ; j<newDefects.size() ; j++) {
            startIDx = newDefects[i][0];
            Point ptStart(contours[contourIDx][startIDx]);

            endIDx = newDefects[i][1];
            Point ptEnd(contours[contourIDx][endIDx]);

            startIDx2 = newDefects[j][0];
            Point ptStart2(contours[contourIDx][startIDx2]);

            endIDx2 = newDefects[j][1];
            Point ptEnd2(contours[contourIDx][endIDx2]);

            if(distanceP2P(ptStart, ptEnd2) < tolerance) {
                contours[contourIDx][startIDx] = ptEnd2;
                break;
            }
            if(distanceP2P(ptEnd, ptStart2) < tolerance) {
                contours[contourIDx][startIDx2] = ptEnd;
            }
        }
    }
}

void Hand::eleminateDefects(Image *im) {
    int tolerance = bRect_h/5;
    float angleTol = 95;
    vector<Vec4i> newDefects;
    int startIDx, endIDx, farIDx;
    vector<Vec4i>::iterator d = defects[contourIDx].begin();

    while(d != defects[contourIDx].end()) {
        Vec4i &v = (*d);

        startIDx = v[0];
        Point ptStart(contours[contourIDx][startIDx]);

        endIDx = v[1];
        Point ptEnd(contours[contourIDx][endIDx]);

        farIDx = v[2];
        Point ptFar(contours[contourIDx][farIDx]);

        if(
                distanceP2P(ptStart, ptFar) > tolerance &&
                distanceP2P(ptEnd, ptFar) > tolerance &&
                getAngle(ptStart, ptFar, ptEnd) < angleTol) {
            if(
                    ptEnd.y > (
                        boundRect.y +
                        boundRect.height -
                        boundRect.height/4)) {
            }
            else if(ptStart.y > (
                        boundRect.y +
                        boundRect.height -
                        boundRect.height/4)) {
            }
            else {
                newDefects.push_back(v);
            }
        }

        d++;
    }

    nrDefects = newDefects.size();
    defects[contourIDx].swap(newDefects);
    remRedundantEnd(defects[contourIDx], im);
}
