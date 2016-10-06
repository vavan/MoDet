#ifndef DETECTOR_H_
#define DETECTOR_H_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "gridmask.h"
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
        Mat diff;
        int lowThreshold;
        int numberNonZero;
        int percentNonZero;

        bool show;
        string streamUrl;
        string img_path;

        //Create capture object, open stream
        VideoCapture createCapture();

        //Process new frame
        void processFrame(InputArray input, Timer& lastDetected);

        //Handle motion detected event. FRAME is the frame with detected motion
        void detected(Mat& frame);

        //update nonZero limit as a percent of the frame
        void updateNoneZero(Size size);

        //build grid mask used for filtration. SIZE is the size of the frame
        void buildMask(Size size);

        //Format time according to the example:"2014-02-02T20:15:20"
        string getFormattedTime();

	public:
        //Ctor for specific device and session
        MotionDetector();

        //Run detection cycle
        bool run();
};




#endif /* DETECTOR_H_ */
