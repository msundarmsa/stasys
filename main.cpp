#include <iostream>
#include <opencv2/opencv.hpp>
#include "CalibrationThread.cpp"
#include "ShootThread.h"
#include "ShootController.h"
#include "Utils.h"
#include <string>
#include <cstdlib>
#include <unistd.h>

using namespace cv;
using namespace std;

class DummyController : public ShootController {
	public:
		DummyController(){
			// DUMMY INITIALIZER
		}

		void removePreviousCalibCircle() {
			// DUMMY METHOD
		}

		void clearTrace(bool b) {
			// DUMMY METHOD
		}

		void updateView(Shot *shot) {
			cout << "Shot Stats: " << endl;
			cout << "Score: " << shot->getScore() << endl;
			cout << "Angle: " << shot->getAngle() << endl;
			cout << "Stab: " << shot->getStab() << endl;
			cout << "Desc: " << shot->getDesc() << endl;
			cout << "Aim: " << shot->getAim() << endl;
			// cout << "ShotPoint: " << shot->getShotPoint() << endl;
		}

		void addToBeforeShotTrace(Vector2D tracePoint) {
			// DUMMY METHOD
		}

		void addToAfterShotTrace(Vector2D tracePoint) {
			// DUMMY METHOD
		}

		void drawShotCircle(Vector2D shotPoint) {
			// DUMMY METHOD
		}
};

void calibrationFinished(bool success, double vecX, double vecY,
		double radius, int frameWidth, int frameHeight) {

	if (success) {
		cout << "SUCCESS" << endl;
		cout << "Adjustment Vector: (" << vecX << " , " << vecY << ")" << endl;
		cout << "Radius: " << radius << endl;
		cout << "Frame Size: " << frameWidth << " x " << frameHeight <<endl;
		cout << "./stasys-cli -s " << vecX << "," << vecY << "," << radius << " (-v path_to_video | -w webcam_id)" << endl;
	} else {
		cout << "FAILURE" << endl;
	}
}

int main(int argc, char *argv[]) {
	cout << "STASYS CLI v1.0" << endl;
	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int hflag = 0, cflag = 0, sflag = 0, vflag = 0, lflag = 0, wflag = 0;
	char *log_file_path = NULL, *calibrate_args = NULL, *video_file_path = NULL, *webcam_id = NULL;
	FILE* log_file = stdout;
	static char usage[] = " [-l path_to_log] [-c | -s calibrateX,calibrateY,calibrateRadius] [-v path_to_video | -w webcam_id]";

	while((c = getopt(argc, argv, "hcl:s:v:w:")) != -1) {
		switch(c) {
			case 'h':
				hflag = 1;
				break;
			case 'c':
				cflag = 1;
				break;
			case 'l':
				lflag = 1;
				log_file_path = optarg;
				break;
			case 's':
				sflag = 1;
				calibrate_args = optarg;
				break;
			case 'v':
				vflag = 1;
				video_file_path = optarg;
				break;
			case 'w':
				wflag = 1;
				webcam_id = optarg;
				break;
			case '?':
				err = 1;
				break;
		}
	}

	if (hflag == 1) {
		cerr << "USAGE: " << argv[0] << usage << endl;
		return -1;
	}

	if (cflag == 0 && sflag == 0) {
		cerr << argv[0] << ": missing -c | -s calibrateX,calibrateY,calibrateRadius option" << endl;
		return -1;
	}

	if (vflag == 0 && wflag == 0) {
		cerr << argv[0] << ": missing -v video_file_path | -w webcam_id option" << endl;
		return -1;
	}

	if (err) {
		cerr << "USAGE: " << argv[0] << usage << endl;
		return -1;
	}

	if (lflag == 1) {
		log_file = fopen(log_file_path, "w");
		if (log_file == NULL) {
			cerr << "log file at " << log_file_path << " cannot be opened using stdout instead" << endl;
		}

		log_file = stdout;
	}

	double radius = -1.0;
	Vector2D adjustmentVec = Vector2D { 0.0, 0.0 };
	if (sflag == 1) {
		stringstream ss(calibrate_args);
		string token;
		int i = 0;
		while(getline(ss, token, ',')) {
			if (i == 0) {
				adjustmentVec.x = strtod(token.c_str(), NULL);
			} else if (i == 1) {
				adjustmentVec.y = strtod(token.c_str(), NULL);
			} else if (i == 2) {
				radius = strtod(token.c_str(), NULL);
			} else {
				cerr << "-s option only expects 3 arguments calibrateX,calibrateY,calibrateRadius discarding rest" << endl;
				break;
			}

			i++;
		}

		if (radius == -1.0) {
			cerr << "-s option expects 3 arguments calibrateX,calibrateY,calibrateRadius but only " << i << " given";
			return -1;
		}
	}

	VideoCapture cap;
	if (vflag) {
		cap.open(video_file_path);
		if (!cap.isOpened()) {
			cerr << "Video file " << video_file_path << " cannot be read!" << endl;
			return -1;
		}
	} else {
		cap.open(atoi(webcam_id));
		if (!cap.isOpened()) {
			cerr << "Webcam " << webcam_id << " cannot be opened!" << endl;
			return -1;
		}
	}

	DummyController controller;

	if (cflag) {
		CalibrationThread thread(cap, calibrationFinished, log_file);
		cout << "Processing started..." << endl;
		thread.start();
		thread.join();
		cout << "Processing ended!" << endl;
	} else {
		ShootThread thread(cap, radius, adjustmentVec, &controller, log_file);
		cout << "Processing started..." << endl;
		thread.start();
		thread.join();
		cout << "Processing ended!" << endl;
	}

}
