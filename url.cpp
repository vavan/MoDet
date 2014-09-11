#include <curl/curl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "url.h"
#include "tool.h"


//Motion detection event ID
#define ALERT_TYPE_MOTION "1"

Json::Value json_parse(std::string raw_data)
{
    Json::Value root;
    Json::Reader reader;

    if ( !reader.parse( raw_data, root ) )
    {
        LOG.error("Json parsing error: "+reader.getFormattedErrorMessages());
        LOG.error("Json input was: "+raw_data);
    }
    return root;
}

Url::Url(string deviceId)
{
	this->deviceId = deviceId;
	this->db_url = (const char*)MDConfig::getRoot()["backend"]["base_url"];
}


size_t Url::callback(void *ptr, size_t size, size_t count, void *stream)
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

    if(curl) 
    {
        CURLcode res;

        for(list<string>::iterator i = headers.begin(); i != headers.end(); i++)
        {
            headerlist = curl_slist_append(headerlist, (*i).c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        if (post)
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        else
        	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        if (isDebug())
        	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Url::callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        	LOG.errorStream() << "curl_easy_perform() failed: " << curl_easy_strerror(res);

        curl_easy_cleanup(curl);

        curl_slist_free_all(headerlist);
    }
    return response;
}

void Url::pushDb(string time, string imgUrl)
{
	const string json = "{\"deviceId\": \""+deviceId+"\""+
			",\"alertType\": "+ALERT_TYPE_MOTION+
			",\"alertData\": \""+imgUrl+"\""+
			",\"alertDate\": \""+time+"\"}}";

	string url = MDConfig::getRoot()["push"]["push_endpoint"];
	url = this->db_url + "/" + url;

	list<string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("sessionid: " + this->sessionId);

	LOG.debugStream() << "Push json: " << json;
	LOG.debugStream() << "Push to url: " << url;
	string response = this->execute(headers, url, json);
	LOG.infoStream() << "Event DB update response: " << response;
}

void Url::push(string time, string imageName) {
	libconfig::Setting& pushCfg = MDConfig::getRoot()["push"];

	string imageUrl = pushCfg["img_url"];
	imageUrl += "/" + imageName;

	pushDb(time, imageUrl);
}

string Url::getGrid()
{
	string url = MDConfig::getRoot()["gridmask"]["mask_endpoint"];
	url = this->db_url + "/" + url + "/" + deviceId;

    LOG.debugStream() << "Get the gridmask from Device URL: " << url;

    list<string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("sessionid: " + this->sessionId);

    return this->execute(headers, url, "", false);
}

string Url::login()
{
	string endpoint = MDConfig::getRoot()["backend"]["login_endpoint"];
	endpoint = this->db_url + "/" + endpoint + "/" + deviceId;

    LOG.debugStream() << "Login to: " << endpoint;

    list<string> headers;
    headers.push_back("Content-Type: application/json");

    string sRoot = this->execute(headers, endpoint, "", false);
    Json::Value root = json_parse(sRoot);
    string sessionid = root.get("sessionid", "").asString();
    LOG.debugStream() << "Got SessionId: " << sessionid;
    return sessionid;
}

void Url::setSessionId(string sessionId)
{
	this->sessionId = sessionId;
}
