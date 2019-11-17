#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

#include <vdr/plugin.h>

using namespace std;

bool FileExists(const string &fullpath);
bool FileExists(const string &path, const string &name, const string &ext);
string ReadFromFile(const string &path);
bool WriteIntoFile(const string &path, const string &content);
time_t FileCreationTime(const string &path);

#endif // __FILESYSTEM_H
