#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include <iostream>
#include <string>
#include <stdlib.h>

#include "config.h"

using namespace std;
//using namespace std;

#define CONFIG_FILE "/etc/modet.cfg"
#define LOG_FILE "/var/log/modet.log"

MDConfig *MDConfig::instance = NULL;

MDConfig::MDConfig()
{
	try {
		cfg.readFile(CONFIG_FILE);
	} catch (const libconfig::FileIOException &fioex) {
		LOG.error("I/O error while reading config file");
		exit(1);
	} catch (const libconfig::ParseException &pex) {
		LOG.errorStream() << "Parse error at " << pex.getFile() << ":"
				<< pex.getLine() << " - " << pex.getError();
		exit(1);
	}
}


void init_log()
{
	log4cpp::Appender *appender = new log4cpp::FileAppender("default", LOG_FILE);
	appender->setLayout(new log4cpp::BasicLayout());

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::DEBUG);
	root.addAppender(appender);
}

