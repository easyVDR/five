#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <time.h>
#include "filesystem.h"

bool FileExists(const string &fullpath) {
    struct stat buffer;
    return (stat (fullpath.c_str(), &buffer) == 0); 
}

bool FileExists(const string &path, const string &name, const string &ext) {
    stringstream fileName;
    fileName << path << name << "." << ext;
    struct stat buffer;
    return (stat (fileName.str().c_str(), &buffer) == 0); 
}

string ReadFromFile(const string &path) {
    string line;
    stringstream content;
    ifstream myfile (path.c_str());
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            content << line << '\n';
        }
        myfile.close();
    }
    return content.str();
}

bool WriteIntoFile(const string &path, const string &content) {
    if (content.size() < 5)
        return false;
    ofstream myfile;
    myfile.open(path.c_str());
    if (myfile.is_open()) {
        myfile << content;
        myfile.close();
        return true;
    }
    return false;
}

time_t FileCreationTime(const string &path) {
    struct stat t_stat;
    stat(path.c_str(), &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_ctime);
    time_t creationTime = mktime(timeinfo);
    return creationTime;
}
