#include <string.h>
#include <math.h>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "HandTracking.h"

using namespace cv;
using namespace std;

#define PI 3.14159

class FingerTipDetection {

public:

	int IndexOfBiggestContour;
	Rect boundRect;
	Mat output;
	HandTracking ht;

	void ContourWindow(vector<Point> contour, Mat &src);
	void Countours(Mat &src_gray, Mat &src);
	void DrawDefects(Mat &src, vector<Vec4i> ConvexityDefects,
			vector<Point> contour);

private:

	int BiggestContour(vector<vector<Point> > contours);
	void Morph(Mat &src);
	float EuclidianDistance(Point a, Point b);
	float getAngle(Point s, Point f, Point e);
	void printDefects(vector<Vec4i> ConvexityDefects);
	void ContourDefects(vector<Vec4i>& ConvexityDefects, vector<Point> contour,
			Mat &src);
	void RemoveRedundantsEndPointsDefects(vector<Vec4i>& ConvexityDefects,
			vector<Point> & contour, Mat &src);

};
