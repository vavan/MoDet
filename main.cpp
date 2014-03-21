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
        int maskLeap;
        bool show;
        string streamUrl;
        string img_path;

        VideoCapture createCapture();
        void processFrame(InputArray input);
        void detected(time_t timestamp, Mat& frame);
        void build_mask(Size size);

	public:
        MotionDetector(string deviceId, string sessionId);
        bool run(Process &process);
};

MotionDetector::MotionDetector(string deviceId, string sessionId): url(deviceId, sessionId) {
    this->deviceId = deviceId;

    libconfig::Setting& r = MDConfig::getRoot();
    libconfig::Setting& m = r["main"];

    this->show = MDConfig::getRoot()["main"]["show"];
    this->timeout = MDConfig::getRoot()["main"]["timeout"];
    this->leap = MDConfig::getRoot()["main"]["leap"];
    this->maskLeap = MDConfig::getRoot()["gridmask"]["leap"];
    string stream = MDConfig::getRoot()["main"]["stream"];
    this->streamUrl = stream + "/" + deviceId + "?sessionid="+sessionId;
    string img_path = MDConfig::getRoot()["push"]["img_path"];
    this->img_path = img_path;
};


VideoCapture MotionDetector::createCapture()
{
    VideoCapture cap(streamUrl);
	LOG.infoStream() << "Cap 1";
    if ( !cap.isOpened() )
    {
		LOG.errorStream() << "Cannot open the video stream: " << streamUrl;
		Process::exit();
    }
	LOG.infoStream() << "Cap 2";
    double fps = cap.get(CV_CAP_PROP_FPS); 
    return cap;
}

string get_time(time_t timestamp)
{
	//"2014-02-02T20:15:20"
	char buffer[20];
	tm* l= gmtime(&timestamp);
	strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S", l);
	string str(buffer);
	return str;
}

void MotionDetector::build_mask(Size size)
{
	string grid = url.get_grid();
	mask = GridMask::create(grid);

	mask.build(size);
}

void MotionDetector::detected(time_t timestamp, Mat& frame)
{
	LOG.debug("!!! Motion detected");
	string stime = get_time(timestamp);
	string imageName = stime + ".jpg";
	imwrite(this->img_path + "/" + imageName, frame);
	url.push(stime, imageName);
}

void MotionDetector::processFrame(InputArray inputFrame)
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
	threshold(diff, diff, 50, 255, THRESH_BINARY);
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

bool MotionDetector::run(Process &process)
{
	LOG.infoStream() << "Start receiving stream from " << this->streamUrl;
	last_detected = time(NULL);
	VideoCapture cap = createCapture();
	LOG.infoStream() << "Cap created";
	if (show) namedWindow("MD window", WINDOW_AUTOSIZE);

	Mat frame;
	int skip = 0; int maskSkip = 0;
	while(process.isRunning())
	{
		if (cap.grab()) {
			if (skip++ > leap)
			{
				if (cap.retrieve(frame)) {
					if (mask.get().empty() || maskSkip++ > maskLeap)
					{
						build_mask(frame.size());
						maskSkip = 0;
					}
					processFrame(frame);
					skip = 0;
				} else {
					LOG.error("retrive() failed");
					return true;
				}
			}
			//if( waitKey (30) == 27 ) break;
			waitKey(20);
		} else {
			LOG.error("grab() failed");
			return true;
		}
	}
    return false;
}



int main(int argc, char**argv) 
{
	//get the device name from argv
    if (argc < 4) {
    	LOG.error("MotDet Error: Wrong command line. USE modet <start|stop> <deviceId> <sessionId>");
    	Process::exit();
    }
    string mode(argv[1]);
    string deviceId(argv[2]);
    string sessionId(argv[3]);

	Process process = Process(deviceId);

    if (mode == "start" || mode == "sync") {
    	if (!process.isLocked()) {
        	LOG.errorStream() << "Process for device '" << deviceId << "' is already running";
        	Process::exit();
    	}
    	if (mode == "sync") {
    		process.start(false);
    	} else {
    		process.start();
    	}

	    init_log(process.getPid(), deviceId);
		LOG.info("Start instance");

		try
		{
			MDConfig::init();

			MotionDetector md = MotionDetector(deviceId, sessionId);
			bool running = true;
			while(running) {
				running = md.run(process);
				LOG.debug("Restart: %d", running);
			}
		}
	    catch(...)
	    {
	        //const char* err_msg = e.what();
	        //LOG.error("OpenCV exception: %s", err_msg);
	    	LOG.error("Unhandled exception!!!");
	    }
		LOG.info("Stop instance");
    } else if (mode == "stop") {
	    init_log(0, deviceId);
		LOG.info("Kill instance");
		process.kill();
    } else {
    	LOG.error("Wrong mode MODE=[start|sync|stop]");
    	Process::exit();
    }
	return 0;
}


