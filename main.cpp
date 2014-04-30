/*
 * Motion Detection System. VZ Smart Home Initiative
 *
 * Dependency:
 *  OpenCV: apt-get install libopencv-dev
 *   Old option:  https://help.ubuntu.com/community/OpenCV
 *  JSON: included
 *  Config: apt-get install libconfig++8-dev
 *  Logging: apt-get install liblog4cpp5-dev
 *  Curl:apt-get install libcurl4-openssl-dev
 *  
 */
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "tool.h"
#include "process.h"
#include "detector.h"

using namespace std;


/*
 * Run detection and handle all the exceptions
 */
void runDetection(string& deviceId) {
	try {
		MotionDetector md = MotionDetector(deviceId);
		bool running = true;
		while (running) {
			running = md.run();
			LOG.debug("Restart: %d", running);
		}
	} catch (cv::Exception& e) {
		const char* err_msg = e.what();
		LOG.error("OpenCV exception: %s", err_msg);
	} catch (libconfig::SettingNotFoundException& e) {
		const char* err_msg = e.what();
		LOG.error("Config exception: %s", err_msg);
	} catch (std::exception& e) {
		const char* err_msg = e.what();
		LOG.error("STD exception: %s", err_msg);
	} catch (...) {
		LOG.error("Unhandled exception!!!");
	}
}

/*
 * Program entry point. Start/stop daemon.
 */
int main(int argc, char**argv) 
{
	init_log();
    if (argc < 3) {
    	LOG.error("MotDet Error: Wrong command line. USE modet <start|stop> <deviceId> <sessionId>");
    	Process::exit();
    }
    string mode(argv[1]);
    string deviceId(argv[2]);

    Process::init(deviceId);

    if (mode == "start" || mode == "sync") {
    	if (!Process::instance().isLocked()) {
        	LOG.errorStream() << "Process for device '" << deviceId << "' is already running";
        	Process::exit();
    	}
    	if (mode == "sync") {
    		Process::instance().start(false);
    	} else {
    		Process::instance().start();
    	}
    	MDConfig::init();
	    init_log(deviceId);

	    LOG.infoStream() << "Start instance. Version: " << version_name();
		runDetection(deviceId);
		LOG.info("Stop instance");

    } else if (mode == "stop") {
	    init_log(deviceId);
		LOG.info("Kill instance");
		Process::instance().kill();
    } else {
    	LOG.error("Wrong mode MODE=[start|sync|stop]");
    	Process::exit();
    }
    Process::done();
	return 0;
}


