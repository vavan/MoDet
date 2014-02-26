//https://help.ubuntu.com/community/OpenCV

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "url.h"
#include "gridmask.h"
#include "config.h"
#include "process.h"


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
        string deviceId;

        int timeout;
        int leap;
        bool show;


        string streamUrl;
        VideoCapture createCapture();
        void process(InputArray input);
        void detected(time_t timestamp, Mat& frame);

	public:
        MotionDetector(Url& url, GridMask& gridmask);
        void setStream(string streamUrl) {
            this->streamUrl = streamUrl;
        };
        void setDeviceId(string deviceId) {
        	this->deviceId = deviceId;
        }
        void run();
};

MotionDetector::MotionDetector(Url& url, GridMask& gridmask) {
    this->url = url;
    this->mask = gridmask;
    this->show = MDConfig::getRoot()["main"]["show"];
    this->timeout = MDConfig::getRoot()["main"]["timeout"];
    this->leap = MDConfig::getRoot()["main"]["leap"];
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

string get_time(time_t timestamp)
{
	//"2014-02-02 20:15:20"
	char buffer[20];
	tm* l= gmtime(&timestamp);
	strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", l);
	string str(buffer);
	return str;
}

void MotionDetector::detected(time_t timestamp, Mat& frame)
{
	LOG.debug("!!! Motion detected");
	url.push(get_time(timestamp), deviceId, "");
}

void MotionDetector::process(InputArray inputFrame) 
{
	Mat currFrameColor = inputFrame.getMat();
	Mat currFrame;

	GaussianBlur(currFrame, currFrame, Size(9, 9), 2);
	cvtColor(currFrameColor, currFrame, CV_RGB2GRAY);

	if (prevFrame.empty()) {
		currFrame.copyTo(prevFrame);
		return;
	}

	absdiff(prevFrame, currFrame, diff);
	threshold(diff, diff, 30, 255, THRESH_BINARY);
	currFrame.copyTo(prevFrame);

	Mat filtered;
	diff.copyTo(filtered, mask.get());

	if (show) imshow("MD window", filtered);
	int nonZero = countNonZero(filtered);
	if (nonZero > 0) {
		time_t now = time(NULL);
		if (now - last_detected > timeout) {
			detected(now, currFrameColor);
			last_detected = now;
		}
	}
}

void MotionDetector::run() 
{
    try 
    {
		LOG.infoStream() << "Start receiving stream from " << streamUrl;
        last_detected = time(NULL);
        VideoCapture cap = createCapture();
        if (show) namedWindow("MD window", WINDOW_AUTOSIZE);

        Mat frame;
        int skip = 0;
        while(!terminated)
        {
            if (!cap.grab()) break;
            if (skip++ > leap)
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
	//get the device name from argv
    if (argc < 4) {
        cerr  << "MotDet Error: Wrong command line. USE modet <start|stop> <deviceId> <sessionId>" << endl;
        exit(1);
    }
    string mode(argv[1]);
    string deviceId(argv[2]);
    string sessionId(argv[3]);


    if (mode == "start") {
		int pid = start_process();
	    init_log(pid, deviceId);
		LOG.info("Start instance");
		MDConfig::init();

		//get gridmask
		Url url;
		string grid = url.get_grid(deviceId, sessionId);
		GridMask mask = GridMask::create(grid);

		//start deamon
		MotionDetector md = MotionDetector(url, mask);
		string stream = MDConfig::getRoot()["stream"]["url"];
		md.setStream(stream);// + "/" + deviceId);
		md.setDeviceId(deviceId);
		md.run();

		LOG.info("Stop instance");
    } else if (mode == "stop") {
	    init_log(0, deviceId);
		LOG.info("Kill instance");
		stop_process(deviceId);
    } else {
        cerr  << "MotDet Error: Wrong MODE. Should be start or stop" << endl;
        exit(1);
    }
	return 0;
}


