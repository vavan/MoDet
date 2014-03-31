#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <list>


using namespace std;


class Url
{
private:
	string db_url;
	string deviceId;
	string sessionId;
    string execute(list<string> headers, string url, string json, bool post = true);

    void pushDb(string time, string imageUrl);
    void pushClient(string time, string imageUrl);

    static size_t callback(void *ptr, size_t size, size_t count, void *stream);
public:
    Url(string deviceId, string sessionid);
    void push(string time, string imageName);
    string get_grid();

    string get_login(string u, string p); //TODO remove
};


#endif // _URL_H_
