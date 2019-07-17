#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

class Config
{
private:
    map<string,list<string>> map;
    list<string> keys;
    void addToMap(string);
    list<string> split(const string&, char);
public:
    Config (string);
    list<string> getKeys();
    list<string> getValues(string);
} ;


#endif // CONFIG_H