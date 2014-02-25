#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "url.h"



size_t url_write_callback(void *ptr, size_t size, size_t count, void *stream) 
{
  ((string*)stream)->append((char*)ptr, 0, size*count);
  return size*count;
}

string Url::execute(list<string> headers, string url, string json)
{
    CURL *curl;
    string response;
    struct curl_slist *headerlist=NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    for(list<string>::iterator i = headers.begin(); i != headers.end(); i++)
    {
        headerlist = curl_slist_append(headerlist, (*i).c_str());
    }

    if(curl) 
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, url_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);

        curl_slist_free_all(headerlist);
    }
    return response;
}



void Url::push()
{
    printf("!!!/n");
/*
    const string json = "{'channels': ['all'], 'data': {'alert': 'Motion detected!'}}";
    const string url = "http://vovatx.us:8080/cgi-bin/test.py"; //https://api.parse.com/1/push

    list<string> headers;
    headers.push_back("X-Parse-Application-Id: nTwB491pWBx9l9VSlEZG7aEmGhbJ2zM4pLZ307XG");
    headers.push_back("X-Parse-REST-API-Key: 6DUwmC3IeNKQ42sLUSFPlp3bHVzZqvGG9FcCZR5S");
    headers.push_back("Content-Type: application/json");

    this->execute(headers, url, json); 
*/
}


/*
    Url url;
    string sid = url.get_login("erik100", "pw");  
    cout << "SID:" << sid << endl;

*/

/*

$ curl -i -X GET -H "Content-Type: application/json" -H "sessionid: 272fedd8-df36-4368-b4de-965bc83936c9" http://71.96.94.57/verizonbackend/v1/api/device/deviceDetails/8484

Response:
HTTP/1.1 200 OK
Server: Apache-Coyote/1.1
Date: Fri, 21 Feb 2014 23:06:13 GMT
Content-Type: application/json
Transfer-Encoding: chunked

{"device":{"id":1,"deviceId":"8484","device_type":"camera","deviceSetup":"{\"motion_grid\" : {\"selections\":[{\"row\":1,\"col\":1},{\"row\":2,\"col\":1},{\"row\":3,\"col\":1},{\"row\":4,\"col\":1},{\"row\":1,\"col\":2},{\"row\":2,\"col\":2},{\"row\":3,\"col\":2},{\"row\":4,\"col\":2},{\"row\":1,\"col\":3},{\"row\":2,\"col\":3},{\"row\":3,\"col\":3},{\"row\":4,\"col\":3},{\"row\":1,\"col\":4},{\"row\":2,\"col\":4},{\"row\":3,\"col\":4},{\"row\":4,\"col\":4}],\"columns\":10,\"rows\":6}}","status":true},"code":0,"message":"Get Device is success.."}



*/



string Url::get_grid(string deviceId)
{
    const string json = "";
    const string url = "http://71.96.94.57/verizonbackend/v1/api/device/deviceDetails/8484";// TODO change to real deviceid

    list<string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("sessionid:272fedd8-df36-4368-b4de-965bc83936c9");//TODO get sid

    return this->execute(headers, url, "");

    //ifstream t("grid.json");
    //string stub((istreambuf_iterator<char>(t)),
    //              istreambuf_iterator<char>());    

    //return stub; //TODO remove
}



string Url::get_login(string u, string p)
{
    list<string> headers;
    headers.push_back("Content-type: application/json");
    string json = "{\"username\": \"" + u + "\", \"password\": \"" + p + "\"}"; 
    const string url = "http://71.96.94.57/verizonbackend/v1/api/user/login";

    string response = this->execute(headers, url, json);
    return response;
}

