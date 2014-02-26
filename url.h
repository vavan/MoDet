#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <list>


using namespace std;


class Url
{
private:
    string execute(list<string> headers, string url, string json, bool post = true);


public:  
    void push(string time, string deviceId, string stream);
    string get_grid(string deviceId, string sessionid);

    string get_login(string u, string p); //TODO remove
};


#endif // _URL_H_
