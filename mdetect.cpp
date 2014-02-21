//https://help.ubuntu.com/community/OpenCV

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "url.h"
#include "gridmask.h"
#include "base.h"


using namespace cv;
using namespace std;



class MotionDetector
{
    private:
        Mat prevFrame;
        GridMask mask;
        Url url;
        Mat diff;
        time_t last_detected;


        string streamUrl;
        VideoCapture createCapture();
        void process(InputArray input);

	public:
        MotionDetector(Url& url, GridMask& gridmask) {
            this->url = url;
            this->mask = gridmask;
        };
        void setStream(string streamUrl) {
            this->streamUrl = streamUrl;
        };
        void run();
};


VideoCapture MotionDetector::createCapture()
{
    VideoCapture cap(streamUrl);
    if ( !cap.isOpened() )
    {
        LOG.error("Cannot open the video stream");
        exit(1);
    }
    double fps = cap.get(CV_CAP_PROP_FPS); 
    return cap;
}


void MotionDetector::process(InputArray inputFrame) 
{
    Mat currFrame = inputFrame.getMat();

    GaussianBlur(currFrame, currFrame, Size(9, 9), 2);
    cvtColor(currFrame, currFrame, CV_RGB2GRAY);

    if (prevFrame.empty()) {
        currFrame.copyTo(prevFrame);
        return;
    }

    absdiff(prevFrame, currFrame, diff);
    threshold(diff, diff, 30, 255, THRESH_BINARY);
    currFrame.copyTo(prevFrame);

    Mat filtered;
    diff.copyTo(filtered, mask.get());

    //imshow("MD window", filtered);
    int nonZero = countNonZero(filtered);
    if (nonZero > 0) 
    {
        time_t now = time(NULL);
        if (now - last_detected > 5) 
        {
             //url.push();
             LOG.debug(">>>>>>> detected");
             last_detected = now;
        }
    }
}

void MotionDetector::run() 
{
    try 
    {
        last_detected = time(NULL);
        VideoCapture cap = createCapture();
        //namedWindow("MD window", WINDOW_AUTOSIZE); 

        Mat frame;
        int skip = 0;
        while(1)
        {
            if (!cap.grab()) break;
            if (skip++ > 5) 
            {
                if (!cap.retrieve(frame)) break;

                if (mask.get().empty()) 
                {
                    mask.build(frame.size());
                }
                process(frame);
                skip = 0;
            }
            if( waitKey (30) == 27 ) break;
        }
    } 
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        LOG.error("OpenCV exception: %s", err_msg);
    }
}




int main(int argc, char**argv) 
{
    configure_log();
	LOG.info("System start");

    //get the device name from argv
    if (argc < 2) 
    {
        cout  << "MotDet Error: Wrong command line" << endl;
        exit(1);
    }
    string deviceId(argv[1]);

    //get gridmask
    Url url;
    string grid = url.get_grid(deviceId);  
    GridMask mask = GridMask::create(grid);

    //start deamon 
    MotionDetector md = MotionDetector(url, mask);
    md.setStream("rtmp://localhost/rtmp/live");
	md.run();
}


