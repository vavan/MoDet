#ifndef PROCESS_H_
#define PROCESS_H_
#include <string>

//#define process_exit(x) Process::exit()

class Process
{
	bool locked;
	bool running;
	int  pidFile;
	std::string deviceId;
	bool lock();
	std::string pidName();
	int pidRead();
	void pidWrite();
public:
	static void exit();

	Process(std::string deviceId);
	~Process();

	bool isLocked();
	bool isRunning();
	int  getPid();
	void start(bool isDaemon = true);
	void kill();


};


#endif /* PROCESS_H_ */
