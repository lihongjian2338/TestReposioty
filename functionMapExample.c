//This is the example about function and map

#include <iostream>
#include <string>
#include <map>
#include<functional>

using namespace std;

using httpRequestHandler = std::function<int(int, int)>;

typedef struct _uriHandler
{
    httpRequestHandler m_handler;
    std::string m_name;
}uriHandler;

int main()
{
    map<int, uriHandler> mapUri;

    int aVal = 2, bVal = 3;

    mapUri.insert(pair<int, uriHandler>(1, {[](int a, int b)->int{return a+b;}, "test_add"}));

    mapUri.insert(pair<int, uriHandler>(2, {[](int a, int b)->int{return a*b;}, "test_multi"}));

    map<int, uriHandler>::iterator iter;

    for(iter = mapUri.begin(); iter != mapUri.end(); iter++)
    {
        cout<<"the value is:"<<iter->second.m_name<<"value:"<<iter->second.m_handler(aVal, bVal)<<endl;
    }
}
