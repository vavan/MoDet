#ifndef TIMER_H_
#define TIMER_H_
#include <sys/time.h>


/*
 * Real time emulation. Calculate the time slices in milliseconds resolution
 */
class Timer
{
private:
	time_t last;
	int timeout;
	//Return timestump since epoch in ms
	long nowMillisec()
	{
	    struct timeval vtime;
	    gettimeofday(&vtime, NULL);
	    return (vtime.tv_sec*1000 + vtime.tv_usec/1000.0);
	};
public:
	//Start the timer
	Timer(int timeout, bool delayedInit = false)
	{
		if (delayedInit)
			last = 0;
		else
			last = nowMillisec();
		this->timeout = timeout;
	};
	//Is time right? Time to start is right?
	bool isTimeTo()
	{
		time_t now = nowMillisec();
		bool _isTimeTo = ((now - last) > timeout);
		if (_isTimeTo)
			last = now;
		return _isTimeTo;
	}
};



#endif /* TIMER_H_ */
