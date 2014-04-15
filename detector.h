#ifndef DETECTOR_H_
#define DETECTOR_H_
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "gridmask.h"
#include "url.h"
#include "timer.h"

using namespace cv;
using namespace std;

/*
 * Major motion detection part - the algorithm. based on OpenCV
 */
class MotionDetector
{
    private:
        Mat prevFrame;
        GridMask mask;
        Url url;
        Mat diff;
        int lowThreshold;
        int numberNonZero;

        string deviceId;
        bool show;
        string streamUrl;
        string img_path;

        //Create capture object, open stream
        VideoCapture createCapture();

        //Process new frame
        void processFrame(InputArray input, Timer& lastDetected);

        //Handle motion detected event. FRAME is the frame with detected motion
        void detected(Mat& frame);

        //build grid mask used for filtration. SIZE is the size of the frame
        void buildMask(Size size);

        //Format time according to the example:"2014-02-02T20:15:20"
        string getFormattedTime();

	public:
        //Ctor for specific device and session
        MotionDetector(string deviceId);

        //Run detection cycle
        bool run();
};




#endif /* DETECTOR_H_ */
