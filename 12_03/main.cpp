#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>

using namespace cv;
using namespace std;

constexpr int CANVAS_SIZE = 729;
constexpr int RECURSION_DEPTH = 5;


void generateCarpet(Mat &image, int startX, int startY, int dimension, int level) {
	if (level == 0) {
		return;
	}
	
	int segment = dimension / 3;
	rectangle(image, Point(startX + segment, startY + segment), Point(startX + 2 * segment, startY + 2 * segment), Scalar(0, 0, 0), FILLED);
	
	#pragma omp parallel for collapse(2)
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			if (row == 1 && col == 1) continue; 
				generateCarpet(image, startX + row * segment, startY + col * segment, segment, level - 1);
		}
	}
}

int main() {
	Mat canvas(CANVAS_SIZE, CANVAS_SIZE, CV_8UC3, Scalar(255, 255, 255));
	generateCarpet(canvas, 0, 0, CANVAS_SIZE, RECURSION_DEPTH);
	
	imshow("Sierpinski Carpet", canvas);
	waitKey(0);
	return 0;
}
