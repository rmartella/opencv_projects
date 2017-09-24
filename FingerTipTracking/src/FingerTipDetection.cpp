#include "FingerTipDetection.h"

int FingerTipDetection::BiggestContour(vector<vector<Point> > contours) {
	int indexBiggestContour = -1;
	int biggestContour = 0;
	for (int i = 0; i < contours.size(); i++) {
		int size = contours[i].size();

		if (size > biggestContour) {
			biggestContour = size;
			indexBiggestContour = i;
		}
	}
	return indexBiggestContour;
}

void FingerTipDetection::Morph(Mat &src) {
	erode(src, src, Mat(), Point(-1, -1));
	dilate(src, src, Mat(), Point(-1, -1), 2);
}

float FingerTipDetection::EuclidianDistance(Point a, Point b) {
	float result = sqrt(fabs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
	return result;

}

float FingerTipDetection::getAngle(Point s, Point f, Point e) {
	float l1 = EuclidianDistance(f, s);
	float l2 = EuclidianDistance(f, e);
	float dot = (s.x - f.x) * (e.x - f.x) + (s.y - f.y) * (e.y - f.y);
	float angle = acos(dot / (l1 * l2));
	angle = angle * 180 / PI;
	return angle;
}

void FingerTipDetection::printDefects(vector<Vec4i> ConvexityDefects) {
	for (unsigned i = 0; i < ConvexityDefects.size(); i++) {
		cout << "Finger " << i + 1 << ":" << ConvexityDefects.at(i) << endl;
	}
}

void FingerTipDetection::ContourDefects(vector<Vec4i>& ConvexityDefects,
		vector<Point> contour, Mat &src) {
	vector<Vec4i> newDefects;
	vector<Vec4i>::iterator d = ConvexityDefects.begin();
	vector<Point2f> fingerDetect;
	vector<Point2f> fingerDefect;

	while (d != ConvexityDefects.end()) {
		float angleTol = 85;
		Vec4i& v = (*d);
		int startIdx = v[0];
		Point ptStart(contour[startIdx]);

		int endIdx = v[1];
		Point ptEnd(contour[endIdx]);

		int farIdx = v[2];
		Point ptFar(contour[farIdx]);

		double depth = static_cast<double>(v[3]) / 256;

		line(src, ptStart, ptEnd, CV_RGB(255, 255, 255), 2);

		if (depth > 20 && depth < 80) {
			if (getAngle(ptStart, ptFar, ptEnd) < angleTol) {
				newDefects.push_back(v);
				circle(src, ptEnd, 10, Scalar(0, 255, 0), 2);
				fingerDetect.push_back(Point2f(ptStart.x, ptStart.y));
				circle(src, ptStart, 10, Scalar(0, 0, 255), 2);
				circle(src, ptFar, 10, Scalar(255, 0, 0), 2);
				fingerDefect.push_back(Point2f(ptFar.x, ptFar.y));
			}
		}

		d++;
	}
	ConvexityDefects.swap(newDefects);
	ht.setFingerDetect(fingerDetect);
}

void FingerTipDetection::ContourWindow(vector<Point> contour, Mat &src) {
	Point2f rect_points[4];
	RotatedRect minRect;

	vector<Point> contours_poly;

	minRect = minAreaRect(Mat(contour));

	approxPolyDP(Mat(contour), contours_poly, 3, true);
	boundRect = boundingRect(Mat(contours_poly));

	rectangle(src, boundRect.tl(), boundRect.br(), CV_RGB(0, 0, 0), 2, 8, 0);

	minRect.points(rect_points);

	for (int j = 0; j < 4; j++) {
		line(src, rect_points[j], rect_points[(j + 1) % 4], CV_RGB(255, 255, 0),
				2, 8);
	}

}

void FingerTipDetection::Countours(Mat &src_gray, Mat & src) {
	RNG rng(12345);

	Morph(src_gray);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(src_gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE,
			Point(0, 0));

	IndexOfBiggestContour = BiggestContour(contours);

	output = Mat::zeros(src_gray.size(), CV_8UC3);

	if (contours.size() > 0) {

		vector<vector<Point>> hull(contours.size());
		vector<vector<Vec4i>> defects(contours.size());
		vector<vector<int>> inthull(contours.size());

		for (size_t i = 0; i < contours.size(); i++) {

			convexHull(Mat(contours[i]), hull[i], false);
			convexHull(Mat(contours[i]), inthull[i], false);
			convexityDefects(Mat(contours[i]), inthull[i], defects[i]);

			if (IndexOfBiggestContour == i) {

				ContourWindow(contours[i], output);

				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
						rng.uniform(0, 255));
				drawContours(output, contours, i, color, 1, 8, hierarchy, 0,
						Point());
				drawContours(output, hull, i, color, 1, 8, hierarchy, 0,
						Point());
				ContourDefects(defects[i], contours[i], output);
				//RemoveRedundantsEndPointsDefects(defects[i], contours[i], drawing);

			}
		}

		DrawDefects(src, defects[IndexOfBiggestContour],
				contours[IndexOfBiggestContour]);
	}
	ht.doFilter();
	for (int i = 0; i < ht.getFingerPredict().size(); i++) {
		circle(output, ht.getFingerPredict().at(i), 10, Scalar(255, 255, 0), 2);
	}

}

void FingerTipDetection::DrawDefects(Mat &src, vector<Vec4i> ConvexityDefects,
		vector<Point> contour) {
	vector<Vec4i>::iterator d = ConvexityDefects.begin();

	while (d != ConvexityDefects.end()) {
		Vec4i& v = (*d);

		int startIdx = v[0];
		Point ptStart(contour[startIdx]);

		int endIdx = v[1];
		Point ptEnd(contour[endIdx]);

		int farIdx = v[2];
		Point ptFar(contour[farIdx]);

		circle(src, ptEnd, 10, Scalar(0, 255, 0), 2);
		circle(src, ptStart, 10, Scalar(0, 0, 255), 2);
		circle(src, ptFar, 10, Scalar(255, 0, 0), 2);

		d++;
	}

}

void FingerTipDetection::RemoveRedundantsEndPointsDefects(
		vector<Vec4i>& ConvexityDefects, vector<Point> & contour, Mat &src) {
	Vec4i temp;
	int startIdx, endIdx;
	int startIdx2, endIdx2;
	int farIdx, fixIdx;
	vector<Vec4i> newDefects;

	for (int i = 0; i < ConvexityDefects.size(); i++) {
		for (int j = i; j < ConvexityDefects.size(); j++) {
			startIdx = ConvexityDefects[i][0];
			Point ptStart(contour[startIdx]);

			endIdx = ConvexityDefects[i][1];
			Point ptEnd(contour[endIdx]);

			farIdx = ConvexityDefects[i][2];

			fixIdx = ConvexityDefects[i][3];

			startIdx2 = ConvexityDefects[j][0];
			Point ptStart2(contour[startIdx2]);

			endIdx2 = ConvexityDefects[j][1];
			Point ptEnd2(contour[endIdx2]);

			if (EuclidianDistance(ptStart, ptEnd2) > 1) {
				startIdx = endIdx2;
			}
			if (EuclidianDistance(ptEnd, ptStart2) > 1) {
				endIdx = startIdx2;
			}

		}
		temp = Vec4i(startIdx, endIdx, farIdx, fixIdx);
		newDefects.push_back(temp);
	}
	ConvexityDefects.swap(newDefects);

}
