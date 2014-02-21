#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <list>


using namespace std;


class Url
{
private:
    string execute(list<string> headers, string url, string json);


public:  
    void push();
    string get_grid(string deviceId);

    string get_login(string u, string p); //TODO remove
};


#endif // _URL_H_
