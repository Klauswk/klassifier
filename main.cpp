
#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <regex>
#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#include <sys/stat.h>
#define GetCurrentDir getcwd
#endif
#include <list>
using namespace std;

bool regexOk(const string &str, const regex &expression)
{
    if (regex_search(str, expression))
    {
        return true;
    }
    return false;
}

void make_directory(const char *name)
{
#ifdef WIN32
    _mkdir(name);
#else
    mkdir(name, 777);
#endif
}

list<string> listFilesInExecutionFolder()
{
    list<string> listFiles;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(GetCurrentDir(0, 0))) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            listFiles.push_back(string(ent->d_name));
        }
        closedir(dir);
    }
    else
    {
        perror("");
    }

    return listFiles;
}

void checkOrCreateFolder(string folder)
{
    if (access(folder.c_str(), F_OK) == -1)
    {
        make_directory(folder.c_str());
    }
}

bool some(string extension, list<string> list)
{
    for (string elem : list)
    {
        if (elem == extension)
        {
            return true;
        }
    }

    return false;
}

string getFolder(Config config, string fileName)
{
    int pos = fileName.rfind('.');

    if (pos == -1)
    {
        return "";
    }

    string extension = fileName.substr(pos);
    string folder;

    for(string key : config.getKeys()) {
        if(some(extension, config.getValues(key))) {
            folder = key;
        }
    }

    checkOrCreateFolder(folder);

    return folder;
}

bool checkForIgnoredFilenames(list<string> directories, string fileName)
{
    for (string directory : directories)
    {
        if (directory == fileName)
        {
            return true;
        }
    }

    if (fileName.front() == '.')
        return true;

    return false;
}

void move_file(Config config, string fileName)
{
    if (checkForIgnoredFilenames(config.getKeys(), fileName))
    {
        return;
    }

#ifdef WIN32
    //TODO
    GetFullPathName(fileName);
#else
    string folder = getFolder(config, fileName);

    if (folder.size() < 1)
    {
        return;
    }

    std::cout << endl
              << "Moving "
              << fileName
              << " to "
              << folder << endl;

    char *fileLocation = realpath(fileName.c_str(), NULL);
    char *folderLocation = realpath(folder.c_str(), NULL);

    string newLocation(folderLocation);

    newLocation = newLocation + "/" + fileName;

    cout << " old path " << fileLocation << " - new path " << newLocation << endl;

    //int OK = rename(fileLocation, newLocation.c_str());

    /* if(OK != 0) {
        cout << "An error occour:" << endl << errno << endl; 
    }*/
#endif
}

int main()
{
    Config config("config.ini");
    
    list<string> fileList = listFilesInExecutionFolder();

    for (std::list<string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
        move_file(config, *it);

    return 0;
}

#endif // MAIN_H