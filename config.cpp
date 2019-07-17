#ifndef CONFIG_CPP
#define CONFIG_CPP

#include "config.h"

Config::Config(string fileName)
{
    ifstream filein(fileName);

    for (string line; getline(filein, line);)
    {
        //cout << "line: " << line << endl;
        addToMap(line);
    }
}

void Config::addToMap(string str)
{
    int pos = str.find("=");

    if (pos < 1)
    {
        cout << "Couldn't find = in " << str << endl;
        return;
    }

    string key = str.substr(0, pos);
    string value = str.substr(pos + 1);

    cout << key << " - value " << value  << endl;

    mmap[key] = split(value, ',');

    keys.push_back(key);
}

list<string> Config::getKeys()
{
    return keys;
}

list<string> Config::getValues(string key)
{
    return mmap.at(key);
}

list<string> Config::split(const string& s, char delimiter)
{
   list<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

#endif 