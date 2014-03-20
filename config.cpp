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
	} catch (const libconfig::FileIOException &fioex) {
		process_exit("I/O error while reading config file");
	} catch (const libconfig::ParseException &pex) {
		LOG.errorStream() << "Parse error at " << pex.getFile() << ":"
				<< pex.getLine() << " - " << pex.getError();
		process_exit("Stop");
	}
}


void init_log(int pid, string deviceId)
{
	log4cpp::Appender *appender = new log4cpp::FileAppender("default", LOG_FILE);
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	std::stringstream spid; spid << pid;
	layout->setConversionPattern("%d "+spid.str()+" ["+deviceId+".%p] %m%n");
	appender->setLayout(layout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::DEBUG);
	root.addAppender(appender);
	root.info("Version: %s", VERSION);
}


