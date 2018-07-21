#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>



int tst_cam_main(int argc, char* argv[]){
	using namespace std;
    (void)argc; (void)argv;

    cv::VideoCapture cap;
    bool succ = cap.open(0);
    double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    succ = cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    succ = cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    cout << "set: " << succ << endl;
    width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    cout << "width: " << width << endl;

    cv::Mat image;
    if (cap.isOpened()) {
        while (cap.read(image)) {
            cv::imshow("cam", image);
            if (cv::waitKey(10) == 27) 	{
                cout << "ESC pressed -> end video processing" << endl;
                break;
            }
        }
    }


	cout << "Press <enter> to continue" << endl;
	string str;
	getline(cin, str);
	return 0;
}

