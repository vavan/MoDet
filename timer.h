#ifndef TIMER_H_
#define TIMER_H_
#include <sys/time.h>

class Timer
{
private:
	time_t last;
	int timeout;
	long nowMillisec()
	{
	    struct timeval vtime;
	    gettimeofday(&vtime, NULL);
	    return (vtime.tv_sec*1000 + vtime.tv_usec/1000.0);
	};
public:
	Timer(int timeout, bool delayedInit = false)
	{
		if (delayedInit)
			last = 0;
		else
			last = nowMillisec();
		this->timeout = timeout;
	};
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
