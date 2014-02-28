#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "url.h"
#include "config.h"



size_t url_write_callback(void *ptr, size_t size, size_t count, void *stream) 
{
  ((string*)stream)->append((char*)ptr, 0, size*count);
  return size*count;
}

string Url::execute(list<string> headers, string url, string json, bool post)
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
        if (post)
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        else
        	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        bool verbose = MDConfig::getRoot()["main"]["verbose"];
        if (verbose) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
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



void Url::push(string time, string deviceId, string imgageName) {
	libconfig::Setting& pushCfg = MDConfig::getRoot()["push"];
	string imgUrl = pushCfg["img_url"];
	imgUrl += "/" + imgageName;

	const string json = "{\"channels\": [\""+deviceId+"\"], \"data\": {"
			"\"alert\": \"Motion detected!\""
			",\"time\": \""+time+"\""
			",\"device\": \""+deviceId+"\""+
			",\"picture\": \""+imgUrl+"\"}}";


	const string url = pushCfg["url"];

	list<string> headers;
	libconfig::Setting& headers_cfg = pushCfg["headers"];
	int count = headers_cfg.getLength();
	for (int i = 0; i < count; ++i) {
		string header = headers_cfg[i];
		headers.push_back(header);
		LOG.debugStream() << "Push header: " << header;
	}
	headers.push_back("Content-Type: application/json");

	LOG.debugStream() << "Push json: " << json;
	LOG.debugStream() << "Push to url: " << url;
	string response = this->execute(headers, url, json);
	LOG.debugStream() << "Response: " << response;
}

string Url::get_grid(string deviceId, string sessionId)
{
	if (sessionId.empty()) {
		string s = MDConfig::getRoot()["gridmask"]["sid"];
		sessionId = s;
	}

	string urlBase = MDConfig::getRoot()["gridmask"]["url"];
    const string url = urlBase + "/" + deviceId;

    LOG.debugStream() << "Get the gridmask from Device URL: " << url
    		<< ". Session: " << sessionId;

    list<string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("sessionid:" + sessionId);

    return this->execute(headers, url, "", false);
}


/*
string Url::get_login(string u, string p)
{
    list<string> headers;
    headers.push_back("Content-type: application/json");
    string json = "{\"username\": \"" + u + "\", \"password\": \"" + p + "\"}"; 
    const string url = "http://71.96.94.57/verizonbackend/v1/api/user/login";

    string response = this->execute(headers, url, json);
    return response;
}

*/
