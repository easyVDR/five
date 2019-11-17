#ifndef __JSONHELPERS_H
#define __JSONHELPERS_H

#include <string>
#include <sstream>
#include <jansson.h>

using namespace std;

int JsonGetInteger(json_t *jNode, string name);
float JsonGetFloat(json_t *jNode, string name);
string JsonGetString(json_t *jNode, string name);

string FloatToString(float f);
string IntToString(int i);

#endif //__JSONHELPERS_H