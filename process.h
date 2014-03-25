#ifndef PROCESS_H_
#define PROCESS_H_
#include <string>


class Process
{
private:
	bool locked;
	bool running;
	int  pidFile;
	std::string deviceId;

	bool lock();
	std::string pidFileName();
	int pidRead();
	void pidWrite();
	Process(std::string deviceId);

	static Process *_instance;

public:
	static void exit();
	static void terminationHandler(int sign);


	static Process& instance();
	static void init(std::string deviceId);
	static void done();

	~Process();

	bool isLocked();
	bool isRunning();
	int  getPid();
	void start(bool isDaemon = true);
	void kill();


};


#endif /* PROCESS_H_ */
