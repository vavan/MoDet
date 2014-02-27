#include <iostream>
#include <sstream>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "process.h"
#include "config.h"

using namespace std;

int terminated;

string exec(string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    	return "";
    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

void termination_handler(int param)
{
	LOG.debug("Got signal %d", param);
	terminated = 1;
}

void init_signals()
{

	terminated = 0;
	signal(SIGINT, termination_handler);
	signal(SIGTERM, termination_handler);
}

int start_process()
{
	pid_t process_id = 0;

	process_id = fork();
	if (process_id < 0)	{
		cerr << "Can't fork" << endl;
		exit(1);
	}
	if (process_id > 0)	{
		// PARENT PROCESS. Exit normally.
		exit(0);
	}
	// CHILD PROCESS. We are in the daemon.
	umask(0);
	pid_t sid = setsid();
	if(sid < 0)
	{
		cerr << "Can't setsid" << endl;
		exit(1);
	}
	chdir("/");
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	init_signals();
	return getpid();
}

int start_process_sync()
{
	init_signals();
	return getpid();
}

void stop_process(string deviceId)
{
	string cmd = "ps ax | grep \"modet start "+deviceId+"\" | grep -v \"grep\"| awk '{print $1}'";
	string lines = exec(cmd);
	istringstream iss(lines);
    do
    {
        string pid;
        iss >> pid;
        if (!pid.empty()) {
        	string cmd = "kill "+pid;
        	int res = system(cmd.c_str());
        	LOG.debugStream() << "Execute:" << cmd << ". Result: " << res;
        }
    } while (iss);
}

