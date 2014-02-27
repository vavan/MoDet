#ifndef PROCESS_H_
#define PROCESS_H_
#include <string>

extern int terminated;

int start_process();
int start_process_sync();
void stop_process(std::string deviceId);

#endif /* PROCESS_H_ */
