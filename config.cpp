#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/Priority.hh"
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "version.h"
#include "process.h"

using namespace std;


#define CONFIG_FILE "/etc/modet.cfg"
#define LOG_FILE "/var/log/modet.log"

MDConfig *MDConfig::instance = NULL;

MDConfig::MDConfig()
{
	try {
		cfg.readFile(CONFIG_FILE);
		string version = cfg.getRoot()["main"]["version"];
		if (version != CONFIG_VERSION) {
			LOG.error("Config file outdated!");
			Process::exit();
		}
	} catch (const libconfig::FileIOException &fioex) {
		LOG.error("I/O error while reading config file");
		Process::exit();
	} catch (const libconfig::ParseException &pex) {
		LOG.errorStream() << "Config parse error at " << pex.getFile() << ":"
				<< pex.getLine() << " - " << pex.getError();
		Process::exit();
	}
}


void init_log(string deviceId)
{
	log4cpp::Appender *appender = new log4cpp::FileAppender("default", LOG_FILE);
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	std::stringstream spid; spid << getpid();
	layout->setConversionPattern("%d "+spid.str()+"-"+deviceId+" [%p] %m%n");
	appender->setLayout(layout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.removeAllAppenders();
	root.setPriority(log4cpp::Priority::DEBUG);
	root.addAppender(appender);
	root.info("Version: %s", VERSION);
}


