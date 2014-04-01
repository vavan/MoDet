#ifndef PROCESS_H_
#define PROCESS_H_
#include <string>

/*
 * Process management class
 */
class Process
{
private:
	bool locked;
	bool running;
	int  pidFile;
	std::string deviceId;
	static Process *_instance;

	//Lock the PID file
	bool lock();
	//Return name for PID file (the name is based on this->deviceId)
	std::string pidFileName();
	//Read the PID from file
	int pidRead();
	//Write PID into file
	void pidWrite();
	//Ctor. Protected because it is singletone
	Process(std::string deviceId);

public:
	//Dtor
	~Process();

	//Exit abnormally
	static void exit();
	//Callback for SIGTERM
	static void terminationHandler(int sign);

	//Signleton
	static Process& instance();
	static void init(std::string deviceId);
	static void done();

	//Is the process runnig and the PID file locked?
	bool isLocked();
	//Is the process running?
	bool isRunning();
	// Process ID (PID)
	int  getPid();
	//Start the process (isDaemon means run as Linux daemon)
	void start(bool isDaemon = true);
	//Kill instance for device this->deviceId
	void kill();
};


#endif /* PROCESS_H_ */
