/*
 * HandTracking.cpp
 *
 *  Created on: 05/01/2016
 *      Author: rey
 */

#include "HandTracking.h"

HandTracking::HandTracking() {
	// TODO Auto-generated constructor stub

}

HandTracking::~HandTracking() {
	// TODO Auto-generated destructor stub
}

void HandTracking::matching(vector<Point2f> source, vector<Point2f> mattcher,
		int * CMIN) {

	int sizeD = source.size();
	int sizeP = mattcher.size();

	//int C[sizeD], limits[sizeD];
	int * C = new int[sizeD];
	int * limits = new int[sizeD];
	int indexA = sizeD - 1, indexB = sizeD - 1;
	float minDist = -1.0;

	for (int i = 0; i < sizeD; i++) {
		C[i] = i;
	}
	for (int i = sizeP - 1, j = sizeD - 1; i >= sizeP - sizeD; i--, j--) {
		limits[j] = i + 1;
	}

	int topIndex = sizeD - 1;

	while ((indexA > 0 && sizeD > 1) || (indexA >= 0 && sizeD == 1)) {
		while (indexB < limits[indexA]) {
			C[indexA] = indexB++;
			float dA = 0.0;
			for (int k = 0; k < sizeD; k++) {
				dA += cv::norm(mattcher.at(C[k]) - source.at(k));
			}
			if (minDist < 0.0f || dA < minDist) {
				minDist = dA;
				for (int k = 0; k < sizeD; k++) {
					CMIN[k] = C[k];
				}
			}
		}
		if (indexA > 0) {
			int value = C[indexA - 1];
			int offset = 1;
			if (value + 1 < C[indexA]) {
				for (int i = indexA - 1; i < sizeD; i++) {
					int newIndex = value + offset++;
					C[i] = newIndex;
					indexB = newIndex;
				}
				indexA = sizeD - 1;
			} else {
				indexA--;
				indexB = C[indexA] + 1;
			}

			if (indexA - 1 < topIndex)
				topIndex--;
		} else
			indexA--;
	}

	delete[] C;
	delete[] limits;

	/*std::cout << "--- Best permutation --- " << std::endl;
	 std::cout << "Value:" << minDist << std::endl;
	 for (int k = 0; k < sizeD; k++) {
	 std::cout << k << "-" << CMIN[k] << ",";
	 }
	 std::cout << std::endl;*/

}

void HandTracking::doFilter() {

	/*std::cout << "old size vector:" << fskfs.size() << std::endl;
	 std::cout << "---ptrs vector---" << std::endl;
	 for (int i = 0; i < this->fskfs.size(); i++) {
	 std::cout << this->fskfs.at(i) << ",";
	 }
	 std::cout << std::endl;*/

	TypeMatcher typeMatcher;
	int sizePermutation, sizeInf, sizeSup;
	if (fingerPredict.size() >= fingerDetect.size()) {
		sizePermutation = fingerDetect.size();
		sizeInf = fingerDetect.size();
		sizeSup = fingerPredict.size();
	} else {
		sizePermutation = fingerPredict.size();
		sizeInf = fingerPredict.size();
		sizeSup = fingerDetect.size();
	}
	//int bp[sizePermutation];
	int * bp = new int[sizePermutation];

	if (fingerPredict.size() >= fingerDetect.size()) {
		typeMatcher = TypeMatcher::MORE_PREDICT_OR_EQUAL_THAT_DETECT;
		matching(fingerDetect, fingerPredict, bp);
	} else {
		typeMatcher = TypeMatcher::MORE_DETECT_THAT_PREDICT;
		matching(fingerPredict, fingerDetect, bp);
	}

	//std::cout << "---To KF Correct---" << std::endl;
	for (int i = 0; i < sizeInf; i++) {
		cv::Mat meas = cv::Mat(FingerKalmanFilter::measSize, 1,
				FingerKalmanFilter::type);
		int indexDetect = 0;
		int indexPredic = 0;
		if (typeMatcher == TypeMatcher::MORE_PREDICT_OR_EQUAL_THAT_DETECT) {
			indexDetect = i;
			indexPredic = bp[i];
		} else {
			indexDetect = bp[i];
			indexPredic = i;
		}
		meas.at<float>(0) = fingerDetect.at(indexDetect).x;
		meas.at<float>(1) = fingerDetect.at(indexDetect).y;
		/*std::cout << "(" << fingerDetect.at(indexDetect).x << ","
		 << fingerDetect.at(indexDetect).y << "),";*/

		fskfs.at(indexPredic).correctKalmanFilter(meas);
	}
	//std::cout << std::endl;

	//int notapply[sizeSup - sizeInf];
	int * notapply = new int[sizeSup - sizeInf];
	int cnf = 0;
	for (int i = 0; i < sizeSup; i++) {
		bool find = false;
		for (int j = 0; j < sizeInf; j++) {
			if (i == bp[j]) {
				find = true;
				break;
			}
		}
		if (!find)
			notapply[cnf++] = i;
	}

	/*if (typeMatcher == MORE_DETECT_THAT_PREDICT)
	 std::cout << "--- To create ---" << std::endl;
	 else
	 std::cout << "--- To Analize ---" << std::endl;*/
	for (int i = 0; i < sizeSup - sizeInf; i++) {
		if (typeMatcher == MORE_DETECT_THAT_PREDICT) {
			FingerKalmanFilter fnew =
					FingerKalmanFilter::createHandKalmanFilter(
							fingerDetect.at(notapply[i]));
			fskfs.insert(fskfs.begin() + notapply[i], fnew);
		} else {
			std::cout << "Element 1,1 Matrix cov:"
					<< fskfs.at(notapply[i]).getKf().errorCovPost.at<float>(0)
					<< std::endl;
			if (fskfs.at(notapply[i]).getKf().errorCovPost.at<float>(0)
					> 0.8f) {
				fskfs.erase(fskfs.begin() + notapply[i]);
				fingerPredict.erase(fingerPredict.begin() + notapply[i]);
				for (int j = i; j < sizeSup - sizeInf; j++) {
					notapply[j] = notapply[j] - 1;
				}
			}
		}
	}

	fingerPredict.erase(fingerPredict.begin(), fingerPredict.end());
	vector<cv::Point2f> updatePredict;
	for (int i = 0; i < fskfs.size(); i++) {
		fskfs.at(i).predictKalmanFilter();
		cv::Mat state = fskfs.at(i).getState();
		/*std::cout << "Prediction(" << state.at<float>(0) << ","
		 << state.at<float>(1) << ")" << std::endl;*/
		fingerPredict.push_back(
				cv::Point2f(state.at<float>(0), state.at<float>(1)));
	}

	delete[] bp;
	delete[] notapply;

}
