#include "detector.h"
#include "config.h"
#include "process.h"
#include "timer.h"




MotionDetector::MotionDetector(string deviceId, string sessionId): url(deviceId, sessionId) {
    this->deviceId = deviceId;

    this->show = MDConfig::getRoot()["debug"]["show"];
    string stream = MDConfig::getRoot()["stream"];
    this->streamUrl = stream + "/" + deviceId + "?sessionid="+sessionId;
    this->img_path = (const char*)MDConfig::getRoot()["push"]["img_path"];
};


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

string getFormattedTime()
{
	//"2014-02-02T20:15:20"
	time_t now = time(NULL);
	char buffer[20];
	tm* l= gmtime(&now);
	strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S", l);
	string str(buffer);
	return str;
}

void MotionDetector::buildMask(Size size)
{
	string grid = url.get_grid();
	mask = GridMask::create(grid);
	mask.build(size);
}

void MotionDetector::detected(Mat& frame)
{
	LOG.warn("!!! Motion detected");
	string stime = getFormattedTime();
	string imageName = stime + ".jpg";
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
	threshold(diff, diff, 50, 255, THRESH_BINARY);
	currFrame.copyTo(prevFrame);

	Mat filtered;
	diff.copyTo(filtered, mask.get());

	if (show) imshow("MD window", filtered);
	int nonZero = countNonZero(filtered);
	if (nonZero > 0) {
		if (detection_timeout.isTimeTo()) {
			detected(currFrameColor);
		}
	}
}

bool MotionDetector::run()
{
	LOG.infoStream() << "Start receiving stream from " << this->streamUrl;
	VideoCapture cap = createCapture();
	LOG.infoStream() << "Capture object created";
	if (show) namedWindow("MD window", WINDOW_AUTOSIZE);

	Mat frame;

	Timer skip_ms = Timer(MDConfig::getRoot()["debug"]["skip_ms"]);
	Timer mask_ms = Timer(MDConfig::getRoot()["debug"]["mask_ms"]);
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
				if (mask.get().empty() || mask_ms.isTimeTo()) {
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
