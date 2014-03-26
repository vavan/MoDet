#ifndef DETECTOR_H_
#define DETECTOR_H_
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "gridmask.h"
#include "url.h"
#include "timer.h"

using namespace cv;
using namespace std;


class MotionDetector
{
    private:
        Mat prevFrame;
        GridMask mask;
        Url url;
        Mat diff;
        string deviceId;

        bool show;
        string streamUrl;
        string img_path;

        VideoCapture createCapture();
        void processFrame(InputArray input, Timer& lastDetected);
        void detected(Mat& frame);
        void buildMask(Size size);

	public:
        MotionDetector(string deviceId, string sessionId);
        bool run();
};




#endif /* DETECTOR_H_ */
