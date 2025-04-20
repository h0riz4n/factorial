#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using namespace cv;

void detectEyesInFace(Mat& faceROI_gray, CascadeClassifier& eyes_cascade, vector<Rect>& eyes) {
	eyes_cascade.detectMultiScale(faceROI_gray, eyes, 1.1, 10, 0, Size(50, 50));
}

void detectSmilesInFace(Mat& faceROI_gray, CascadeClassifier& smile_cascade, vector<Rect>& smiles) {
	smile_cascade.detectMultiScale(faceROI_gray, smiles, 1.24, 15, 0, Size(40, 40));
}

int main() {
	CascadeClassifier face_cascade, eyes_cascade, smile_cascade;

	if (
		!face_cascade.load("./haarcascade_frontalface_default.xml") ||
		!eyes_cascade.load("./haarcascade_eye.xml") ||
		!smile_cascade.load("./haarcascade_smile.xml")
	) {
		cout << "fail to load haar cascade!" << endl;
		return -1;
	}

	VideoCapture cap("./ZUA.mp4");

	if (!cap.isOpened()) {
		cout << "fail to load video!" << endl;
		return -1;
	}

	Mat frame;
	while (cap.read(frame)) {
		if (frame.empty()) break;

		Mat gray;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		equalizeHist(gray, gray);
		GaussianBlur(gray, gray, Size(5, 5), 0);

		vector<Rect> faces;
		face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, Size(150, 150));

		for (const auto& face : faces) {
			rectangle(frame, face, Scalar(255, 0, 0), 2);

			Mat faceROI_gray = gray(face);
			Mat faceROI_color = frame(face);

			vector<Rect> eyes, smiles;

			thread eyesThread(detectEyesInFace, std::ref(faceROI_gray), std::ref(eyes_cascade), std::ref(eyes));
			thread smilesThread(detectSmilesInFace, std::ref(faceROI_gray), std::ref(smile_cascade), std::ref(smiles));

			eyesThread.join();
			smilesThread.join();

			for (const auto& eye : eyes) {
				rectangle(faceROI_color, eye, Scalar(0, 0, 255), 2);
			}

			for (const auto& smile : smiles) {
				rectangle(faceROI_color, smile, Scalar(0, 255, 0), 2);
			}
		}

		imshow("Face detect", frame);
		if (waitKey(30) >= 0) break;
	}

	cap.release();
	destroyAllWindows();
	return 0;
}
