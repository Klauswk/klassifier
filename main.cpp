
#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <Shlobj.h>
#include <shlwapi.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#include <sys/stat.h>
#define GetCurrentDir getcwd
#endif
#include <list>
using namespace std;

void make_directory(const char *name)
{
#ifdef WIN32
    _mkdir(name);
#else
    mkdir(name, 777);
#endif
}

void createConfigFile(string sConfigLocation)
{
    std::ofstream outfile(sConfigLocation);

    outfile << "Programs=.exe,.msi" << std::endl;
    outfile << "Music=.mp3,.aac,.flac,.ogg,.wma,.m4a,.aiff,.wav,.amr" << std::endl;
    outfile << "Videos=.flv,.ogv,.avi,.mp4,.mpg,.mpeg,.3gp,.mkv,.ts,.webm,.vob,.wmv" << std::endl;
    outfile << "Pictures=.png,.jpeg,.gif,.jpg,.bmp,.svg,.webp,.psd,.tiff" << std::endl;
    outfile << "Archives=.rar,.zip,.7z,.gz,.bz2,.tar,.dmg,.tgz,.xz,.iso,.cpio,.deb,.rpm" << std::endl;
    outfile << "Documents=.txt,.pdf,.doc,.docx,.odf,.xls,.xlsx,.ppt,.ptx,.ppsx,.odt,.ods,.md,.json,.csv" << std::endl;

    outfile.close();
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

bool fileExists(string path)
{
    return access(path.c_str(), F_OK) > -1;
}

//copy from some stackoverflow answer
bool makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            int pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else 
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return fileExists(path);

    default:
        return false;
    }
}

void checkOrCreateFolder(string folder)
{
    if (!fileExists(folder))
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

    for (string key : config.getKeys())
    {
        if (some(extension, config.getValues(key)))
        {
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

void moveFile(Config config, string fileName)
{
    if (checkForIgnoredFilenames(config.getKeys(), fileName))
    {
        return;
    }

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

#ifdef WIN32
    TCHAR fileNameWin[4096];
    GetCurrentDirectory(4096, fileNameWin);

    cout << "Win: " << fileNameWin << endl;

    string sFileNameWin = fileNameWin;
    string sFolderNameWin = fileNameWin;

    sFileNameWin = sFileNameWin + "\\" + fileName;
    sFolderNameWin = sFolderNameWin + "\\" + folder;

    const char *fileLocation = sFileNameWin.c_str();
    const char *folderLocation = sFolderNameWin.c_str();

    string newLocation(folderLocation);

    newLocation = newLocation + "\\" + fileName;

    cout << " old path " << fileLocation << " - new path " << newLocation << endl;

    int OK = rename(fileLocation, newLocation.c_str());

    if (OK != 0)
    {
        cout << "An error occour:" << endl
             << errno << endl;
    }
#else

    char *fileLocation = realpath(fileName.c_str(), NULL);
    char *folderLocation = realpath(folder.c_str(), NULL);

    string newLocation(folderLocation);

    newLocation = newLocation + "/" + fileName;

    cout << " old path " << fileLocation << " - new path " << newLocation << endl;

    int OK = rename(fileLocation, newLocation.c_str());

    if (OK != 0)
    {
        cout << "An error occour:" << endl
             << errno << endl;
    }
#endif
}

int main()
{
    string sConfigLocation;
    string sDirConfigLocation;

#ifdef WIN32
    TCHAR szPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL,
                                  CSIDL_LOCAL_APPDATA,
                                  NULL,
                                  0,
                                  szPath)))
    {
        PathAppend(szPath, TEXT("Klassifier"));

        sDirConfigLocation = szPath;

        PathAppend(szPath, TEXT("config.ini"));

        sConfigLocation = szPath;
    }
#else
    const char *configDir;

    if ((configDir = getenv("XDG_CONFIG_HOME")) == NULL)
    {
        configDir = getenv("HOME");
        sConfigLocation = configDir;
        if(!(sConfigLocation.at(sConfigLocation.size() -1) == '/')) {
            sConfigLocation = sConfigLocation + "/";
        }
        cout << "at: " << sConfigLocation.at(sConfigLocation.size() -1) << endl;

        sDirConfigLocation = sConfigLocation + ".config/Klassifier/";
        sConfigLocation = sDirConfigLocation + "config.ini";
    } else {
        sConfigLocation = configDir;
        if(!(sConfigLocation.at(sConfigLocation.size() -1) == '/')) {
            sConfigLocation = sConfigLocation + "/";
        }
        cout << "at: " << sConfigLocation.at(sConfigLocation.size() -1) << endl;

        sDirConfigLocation = sConfigLocation + "Klassifier/";
        sConfigLocation = sDirConfigLocation + "config.ini";
    }
    
#endif
    if (!fileExists(sConfigLocation))
    {
        makePath(sDirConfigLocation);
        cout << "Couldn't find config file, creating one at " << sConfigLocation << endl;
        createConfigFile(sConfigLocation);
    }

    Config config(sConfigLocation);

    list<string> fileList = listFilesInExecutionFolder();

    for (std::list<string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
        moveFile(config, *it);

    return 0;
}

#endif // MAIN_H