#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <list>
#include "json/json.h"


using namespace std;


//Parse given JSON object
Json::Value json_parse(string raw_data);

/*
 * URL management class. Used for get configuration/data form backend.
 * Based on LIB-CURL
 */
class Url
{
private:
	string db_url;
	string deviceId;
    string sessionId;

	//Execute a request using headers, url, json to send to and mode
    string execute(list<string> headers, string url, string json, bool post = true);

    //Push notification (to DB)
    void pushDb(string time, string imageUrl);

    //Push notofication to client (phone)
    void pushClient(string time, string imageUrl);

    //Url processing callback
    static size_t callback(void *ptr, size_t size, size_t count, void *stream);
public:

    //Ctor
    Url(string deviceId);

    //Push mption detected event notification
    void push(string time, string imageName);

    //Fetch the grid mask from server
    string getGrid();

    //Login and get Session ID from DB
    string login();

    void setSessionId(string sessionId);
};


#endif // _URL_H_
