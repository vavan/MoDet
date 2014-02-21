#include <string>
#include <list>


using namespace std;


class Url
{
private:
    string execute(list<string> headers, string url, string json);


public:  
    void push();
    string get_mask(string sessionId);
    string get_login(string u, string p);
};
