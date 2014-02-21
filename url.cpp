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
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
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


string Url::get_grid(string deviceId)
{
    const string json = "";
    const string url = "http://71.96.94.57/verizonbackend/v1/api/device/devices";// TODO add getGrid API

    list<string> headers;
    headers.push_back("Content-Type: application/json");

    //return this->execute(headers, url, "");

    ifstream t("grid.json");
    string stub((istreambuf_iterator<char>(t)),
                  istreambuf_iterator<char>());    

    return stub; //TODO remove
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

