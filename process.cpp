#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include "process.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

void termination_handler(int param)
{
	LOG.debug("Got signal %d", param);
	//terminated = 1;
	//TODO
}

Process::Process(string deviceId)
{
	this->deviceId = deviceId;
	this->locked = lock();
	this->running = true;
	//signal(SIGINT, termination_handler);
	signal(SIGTERM, termination_handler);
}
Process::~Process()
{
}

string Process::pidName()
{
	return "/var/run/modet/"+this->deviceId+".pid";
};

bool Process::isRunning()
{
	return running;
}
bool Process::isLocked()
{
	return locked;
}


void Process::start(bool isDaemon)
{
	if (isDaemon) {
		pid_t pid = 0;

		pid = fork();
		if (pid < 0)	{
			process_exit("Can't fork");
		}
		if (pid > 0)	{
			// PARENT PROCESS. Exit normally.
			exit(0);
		}
		// CHILD PROCESS. We are in the daemon.
		umask(0);
		pid_t sid = setsid();
		if(sid < 0)
		{
			process_exit("Can't setsid");
		}
		chdir("/");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
	this->pidWrite();
}

bool Process::lock()
{
	const char *pid_file_name = this->pidName().c_str();
	this->pidFile = open(pid_file_name, O_CREAT | O_RDWR, 0666);
	int rc = flock(this->pidFile, LOCK_EX | LOCK_NB);
	if (rc) {
	    if (EWOULDBLOCK == errno) {
	    	// another instance is running
	    	return false;
	    }
	}
	// this is the first instance
	return true;
}

int Process::pidRead()
{
	const int BUFFER_LENGTH = 256;
	char buffer[BUFFER_LENGTH];
	buffer[0] = '\0';
	read(this->pidFile, buffer, BUFFER_LENGTH);
	return atoi(buffer);
}

void Process::pidWrite()
{
	const int BUFFER_LENGTH = 256;
	char buffer[BUFFER_LENGTH];
	sprintf(buffer, "%d", this->getPid());
	int length = strlen(buffer);
	int size = write(this->pidFile, buffer, length);
}

int Process::getPid()
{
	return getpid();
}

void Process::kill()
{
	int pid = this->pidRead();
	if (pid != 0) {
		LOG.debugStream() << "Kill instance: " << pid << " for device: " << deviceId;
		::kill(pid, SIGTERM);
	} else {
		LOG.errorStream() << "No instance for device: " << deviceId;
	}
}

void process_exit(std::string msg)
{
	LOG.error(msg);
	cerr << msg << endl;
	exit(1);
}

