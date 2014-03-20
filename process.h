#ifndef PROCESS_H_
#define PROCESS_H_
#include <string>


class Process
{
	bool locked;
	bool running;
	int pidFile;
	std::string deviceId;
	bool lock();
	std::string pidName();
	int pidRead();
	void pidWrite();
public:
	Process(std::string deviceId);
	~Process();

	bool isLocked();
	bool isRunning();
	int getPid();
	void start(bool isDaemon = true);

	void kill();
};

/*
extern int terminated;

int process_start_daemon();
int process_start_sync();
int process_lock(std::string deviceId);
void process_kill(std::string deviceId);
*/
void process_exit(std::string msg);

#endif /* PROCESS_H_ */
