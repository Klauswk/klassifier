
#ifndef MAIN_H
#define MAIN_H

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

vector<string> arrayOfDirectories = {"Music",
                                     "Videos",
                                     "Pictures",
                                     "Archives",
                                     "Documents",
                                     "Books",
                                     "DEBPackages",
                                     "Programs",
                                     "RPMPackages"};

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
    list<string> listaArquivos;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(GetCurrentDir(0, 0))) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            listaArquivos.push_back(string(ent->d_name));
        }
        closedir(dir);
    }
    else
    {
        perror("");
    }

    return listaArquivos;
}

void checkOrCreateFolder(string folder)
{
    if (access(folder.c_str(), F_OK) == -1)
    {
        make_directory(folder.c_str());
    }
}

bool some(string extension, std::initializer_list<string> list)
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

string getFolder(string fileName)
{

    int pos = fileName.rfind('.');

    if (pos == -1)
    {
        return "";
    }

    string extension = fileName.substr(pos);
    string folder;

    if (some(extension, {".exe", ".msi"}))
    {
        folder = "Programs";
    }
    else if (some(extension, {".mp3", ".aac", ".flac", ".ogg", ".wma", ".m4a", ".aiff", ".wav", ".amr"}))
    {
        folder = "Music";
    }
    else if (some(extension, {".flv", ".ogv", ".avi", ".mp4", ".mpg", ".mpeg", ".3gp", ".mkv", ".ts", ".webm", ".vob", ".wmv"}))
    {
        folder = "Videos";
    }
    else if (some(extension, {".png", ".jpeg", ".gif", ".jpg", ".bmp", ".svg", ".webp", ".psd", ".tiff"}))
    {
        folder = "Pictures";
    }
    else if (some(extension, {".rar", ".zip", ".7z", ".gz", ".bz2", ".tar", ".dmg", ".tgz", ".xz", ".iso", ".cpio", ".deb", ".rpm"}))
    {
        folder = "Archives";
    }
    else if (some(extension, {".txt", ".pdf", ".doc", ".docx", ".odf", ".xls", ".xlsx", ".ppt", ".ptx", ".ppsx", ".odt", ".ods", ".md", ".json", ".csv"}))
    {
        folder = "Documents";
    }

    checkOrCreateFolder(folder);

    return folder;
}

bool checkForIgnoredFilenames(string fileName)
{
    for (string directory : arrayOfDirectories)
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

void move_file(string fileName)
{

    if (checkForIgnoredFilenames(fileName))
    {
        return;
    }

#ifdef WIN32
    //TODO
    GetFullPathName(fileName);
#else
    string folder = getFolder(fileName);

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
    list<string> fileList = listFilesInExecutionFolder();

    for (std::list<string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
        move_file(*it);
}

#endif // CPPTOML_H