#include "opencv2/imgproc/imgproc.hpp"
#include "detector.h"
#include "tool.h"
#include "process.h"
#include "timer.h"


MotionDetector::MotionDetector(string deviceId): url(deviceId) {
    this->deviceId = deviceId;

    this->show = MDConfig::getRoot()["debug"]["show"];
    string stream = MDConfig::getRoot()["stream"];
    this->streamUrl = stream + "/" + deviceId;
    this->img_path = (const char*)MDConfig::getRoot()["push"]["img_path"];

    this->lowThreshold = MDConfig::getRoot()["debug"]["lowThreshold"];
    this->percentNonZero = MDConfig::getRoot()["debug"]["nonZero"];
};

bool MotionDetector::login()
{
	string sessionId = url.login();
    this->streamUrl = this->streamUrl + "?sessionid=" + sessionId;
    url.setSessionId(sessionId);

	return (!sessionId.empty());
}


VideoCapture MotionDetector::createCapture()
{
    VideoCapture cap(streamUrl);
    if ( !cap.isOpened() )
    {
		LOG.errorStream() << "Cannot open the video stream: " << streamUrl;
		Process::exit();
    }
    return cap;
}

string MotionDetector::getFormattedTime()
{
	//Time format is: "2014-02-02T20:15:20"
	time_t now = time(NULL);
	char buffer[20];
	tm* l= gmtime(&now);
	strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S", l);
	string str(buffer);
	return str;
}

void MotionDetector::updateNoneZero(Size size)
{
	int square = size.height * size.width;
	this->numberNonZero = square * this->percentNonZero / 10000;
}

void MotionDetector::buildMask(Size size)
{
	string grid = url.getGrid();
	mask = GridMask::create(grid);
	mask.build(size);
	updateNoneZero(size);
}

void MotionDetector::detected(Mat& frame)
{
	LOG.warn("!!! Motion detected");
	string stime = getFormattedTime();
	string imageName = this->deviceId + "_" + stime + ".jpg";
	imwrite(this->img_path + "/" + imageName, frame);
	url.push(stime, imageName);
}

void MotionDetector::processFrame(InputArray inputFrame, Timer& detection_timeout)
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
	threshold(diff, diff, this->lowThreshold, 255, THRESH_BINARY);
	currFrame.copyTo(prevFrame);

	Mat filtered;
	diff.copyTo(filtered, mask.get());

	if (show) imshow("MD window", filtered);
	int nonZero = countNonZero(filtered);
	//if (nonZero)
	//	LOG.debugStream() << "Non zero: " << nonZero << ", limit: " << this->numberNonZero;
	if (nonZero > this->numberNonZero) {
		if (detection_timeout.isTimeTo()) {
			detected(currFrameColor);
		}
	}
}

bool MotionDetector::run()
{
	LOG.infoStream() << "Start receiving stream from " << this->streamUrl;
	if (!login()) {
		LOG.errorStream() << "Login failed";
		return false;
	}
	VideoCapture cap = createCapture();
	LOG.infoStream() << "Capture object created";
	if (show) namedWindow("MD window", WINDOW_AUTOSIZE);

	Mat frame;

	Timer skip_ms = Timer(MDConfig::getRoot()["debug"]["skip_ms"]);
	Timer mask_ms = Timer(MDConfig::getRoot()["debug"]["mask_ms"], true);
	Timer detection_timeout = Timer(MDConfig::getRoot()["debug"]["detection_timeout"], true);


	while(Process::instance().isRunning())
	{
		if (cap.grab()) {
			if (skip_ms.isTimeTo()) {
				if (cap.retrieve(frame)) {
					processFrame(frame, detection_timeout);
				} else {
					LOG.error("retrive() failed");
					return true;
				}
				if (mask_ms.isTimeTo()) {
					buildMask(frame.size());
				}
			}
			waitKey(10);
		} else {
			LOG.error("grab() failed");
			return true;
		}
	}
    return false;
}
