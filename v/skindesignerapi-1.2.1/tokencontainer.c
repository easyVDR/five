#include "tokencontainer.h"

skindesignerapi::cTokenContainer::cTokenContainer(void) {
    numIntTokens = 0;
    numStringTokens = 0;
    stringTokens = NULL;
    intTokens = NULL;
    stNames = NULL;
    itNames = NULL;
}

skindesignerapi::cTokenContainer::cTokenContainer(const cTokenContainer &other) {
    numIntTokens = 0;
    numStringTokens = 0;
    stringTokens = NULL;
    intTokens = NULL;
    stNames = NULL;
    itNames = NULL;
    stringTokenNames = other.stringTokenNames;
    intTokenNames = other.intTokenNames;
    loopTokenNames = other.loopTokenNames;
    loopNameMapping = other.loopNameMapping;
}

skindesignerapi::cTokenContainer::~cTokenContainer(void) {
    Clear();
    delete[] intTokens;
    delete[] stringTokens;
    delete[] stNames;
    delete[] itNames;
}

void skindesignerapi::cTokenContainer::CreateContainers(void) {
    numIntTokens = intTokenNames.size();
    if (numIntTokens) {
        intTokens = new int[numIntTokens];
        itNames = new string[numIntTokens];
        for (int i=0; i < numIntTokens; i++) {
            intTokens[i] = -1;
            itNames[i] = GetIntTokenName(i);
        }
    }
    numStringTokens = stringTokenNames.size();
    if (numStringTokens) {
        stringTokens = new char*[numStringTokens];
        stNames = new string[numStringTokens];
        for (int i=0; i < numStringTokens; i++) {
            stringTokens[i] = NULL;
            stNames[i] = GetStringTokenName(i);
        }
    }

    int numLoops = loopTokenNames.size();
    for (int i = 0; i < numLoops; ++i) {
        vector<string> loopToken;
        int numLoopTokens = loopTokenNames[i].size();
        for (int j = 0; j < numLoopTokens; ++j) {
            string tokenName = GetLoopTokenName(i, j);
            loopToken.push_back(tokenName);
        }
        ltNames.push_back(loopToken);
    }
}

void skindesignerapi::cTokenContainer::CreateLoopTokenContainer(vector<int> *loopInfo) {
    int numLoops = loopInfo->size();
    for (int i = 0; i < numLoops; ++i) {
        numLoopTokens.push_back(loopInfo->at(i));
        int rows = loopInfo->at(i);
        char*** loopToks = new char**[rows];
        for (int j = 0; j < rows ; ++j) {
            int numLoopTokens = loopTokenNames[i].size();
            loopToks[j] = new char*[numLoopTokens];
            for (int k = 0; k < numLoopTokens; ++k) {
                loopToks[j][k] = NULL;
            }
        }
        loopTokens.push_back(loopToks);
    }
}

void skindesignerapi::cTokenContainer::DeleteLoopTokenContainer(void) {
    int i = 0;
    for (vector<char***>::iterator it = loopTokens.begin(); it != loopTokens.end(); it++) {
        char*** loopToks = *it;
        for (int j = 0; j < numLoopTokens[i] ; j++) {
            int numToks = loopTokenNames[i].size();
            for (int k = 0; k < numToks; ++k) {
                free(loopToks[j][k]);
            }
            delete[] loopToks[j];
        }
        delete[] loopToks;
        ++i;
    }
    loopTokens.clear();
    numLoopTokens.clear();
}

void skindesignerapi::cTokenContainer::DefineStringToken(string name, int index) {
    stringTokenNames.insert(pair<string, int>(name, index));
}

void skindesignerapi::cTokenContainer::DefineIntToken(string name, int index) {
    intTokenNames.insert(pair<string, int>(name, index));
}

void skindesignerapi::cTokenContainer::DefineLoopToken(string name, int index) {
    string loopName = LoopName(name);
    int loopIndex = LoopIndex(loopName, true);
    if ((int)loopTokenNames.size() < loopIndex+1) {
        map<string, int> newloop;
        newloop.insert(pair<string, int>(name, index));
        loopTokenNames.push_back(newloop);
        return;
    }
    loopTokenNames[loopIndex].insert(pair<string, int>(name, index));
}

int skindesignerapi::cTokenContainer::GetNumDefinedIntTokens(void) {
    return intTokenNames.size();
}

int skindesignerapi::cTokenContainer::LoopIndex(string name, bool createNew) {
    map<string, int>::iterator hit = loopNameMapping.find(name);
    if (hit != loopNameMapping.end())
        return hit->second;
    if (!createNew)
        return -1;
    int index = loopNameMapping.size();
    loopNameMapping.insert(pair<string, int>(name, index));
    return index;
}

int skindesignerapi::cTokenContainer::StringTokenIndex(string name) {
    map<string, int>::iterator hit = stringTokenNames.find(name);
    if (hit == stringTokenNames.end())
        return -1;
    return hit->second;
}

int skindesignerapi::cTokenContainer::IntTokenIndex(string name) {
    map<string, int>::iterator hit = intTokenNames.find(name);
    if (hit == intTokenNames.end())
        return -1;
    return hit->second;
}

int skindesignerapi::cTokenContainer::LoopTokenIndex(string name) {
    string loopName = LoopName(name);
    int loopIndex = LoopIndex(loopName);
    if (loopIndex > -1 && loopIndex < (int)loopTokenNames.size()) {
        map<string, int>::iterator hit = loopTokenNames[loopIndex].find(name);
        if (hit == loopTokenNames[loopIndex].end())
            return -1;
        return hit->second;
    }
    return -1;
}

void skindesignerapi::cTokenContainer::AddIntToken(int index, int value) {
    intTokens[index] = value;
}

void skindesignerapi::cTokenContainer::AddStringToken(int index, const char *value) {
    if (value)
        stringTokens[index] = strdup(value);
}

void skindesignerapi::cTokenContainer::AddLoopToken(int loopIndex, int row, int index, const char *value) {
    if (value) {
        loopTokens[loopIndex][row][index] = strdup(value);
    }
}

int skindesignerapi::cTokenContainer::NumLoops(int loopIndex) { 
    int numLT = numLoopTokens.size();
    if (loopIndex >= 0 && loopIndex < numLT)
        return numLoopTokens[loopIndex];
    return 0; 
}

void skindesignerapi::cTokenContainer::SetTokens(cTokenContainer *other) {
    //Set Int and String Tokens
    if (numIntTokens) {
        for (int i=0; i < numIntTokens; i++) {
            AddIntToken(i, other->IntToken(i));
        }
    }
    if (numStringTokens) {
        for (int i=0; i < numStringTokens; i++) {
            AddStringToken(i, other->StringToken(i));
        }
    }
    //Set Looptoken Container
    set<int> loopIndices;
    for (map<string, int>::iterator it = loopNameMapping.begin(); it != loopNameMapping.end(); it++) {
        loopIndices.insert(it->second);
    }
    vector<int> loopInfo;
    for (set<int>::iterator it = loopIndices.begin(); it != loopIndices.end(); it++) {
        loopInfo.push_back(other->NumLoops(*it));
    }
    CreateLoopTokenContainer(&loopInfo);
    //Set Loop Tokens
    int i = 0;
    for (vector<int>::iterator it = loopInfo.begin(); it != loopInfo.end(); it++) {
        int numRows = *it;
        int numCols = loopTokenNames[i].size();
        for (int j = 0; j < numRows; j++) {
            for (int k = 0; k < numCols; k++) {
                AddLoopToken(i, j, k, other->LoopToken(i, j, k));
            }
        }
        i++;
    }
}

void skindesignerapi::cTokenContainer::Clear(void) {
    if (numIntTokens) {
        for (int i=0; i < numIntTokens; i++) {
            intTokens[i] = -1;
        }
    }
    if (numStringTokens) {
        for (int i=0; i < numStringTokens; i++) {
            free(stringTokens[i]);
            stringTokens[i] = NULL;
        }
    }
    DeleteLoopTokenContainer();
}

void skindesignerapi::cTokenContainer::Debug(void) {
    /*
    esyslog("skindesigner: TokenContainer defined string tokens");
    for (map<string, int>::iterator it = stringTokenNames.begin(); it != stringTokenNames.end(); it++) {
        esyslog("skindesigner: name %s id %d", (it->first).c_str(), it->second);
    }
    esyslog("skindesigner: TokenContainer defined int tokens");
    for (map<string, int>::iterator it = intTokenNames.begin(); it != intTokenNames.end(); it++) {
        esyslog("skindesigner: name %s id %d", (it->first).c_str(), it->second);
    }
    */
    esyslog("skindesigner: TokenContainer content");
    for (int i=0; i < numStringTokens; i++) {
        if (stringTokens[i])
            esyslog("skindesigner: stringtoken %d. %s: \"%s\"", i, stNames[i].c_str(), stringTokens[i]);
        else
            esyslog("skindesigner: stringtoken %d. %s: empty", i, stNames[i].c_str());
    }
    for (int i=0; i < numIntTokens; i++) {
        if (intTokens[i] >= 0)
            esyslog("skindesigner: inttoken %d. %s: %d", i, itNames[i].c_str(), intTokens[i]);
        else
            esyslog("skindesigner: inttoken %d. %s: empty", i, itNames[i].c_str());
    }

    for (size_t i=0; i < loopTokens.size(); i++) {
        for (int j = 0; j < numLoopTokens[i]; j++) {
            esyslog("skindesigner: row %d", j);
            for (size_t k = 0; k < loopTokenNames[i].size(); k++) {
                if (loopTokens[i][j][k])
                    esyslog("skindesigner: looptoken %d. %s: \"%s\"", (int)k, ltNames[i][k].c_str(), loopTokens[i][j][k]);
                else
                    esyslog("skindesigner: looptoken %d. %s: empty", (int)k, ltNames[i][k].c_str());
            }
        }
    }
}


string skindesignerapi::cTokenContainer::GetStringTokenName(int id) {
    for (map<string, int>::iterator it = stringTokenNames.begin(); it != stringTokenNames.end(); it++) {
        if (it->second == id)
            return it->first;
    }
    return "";
}
string skindesignerapi::cTokenContainer::GetIntTokenName(int id) {
    for (map<string, int>::iterator it = intTokenNames.begin(); it != intTokenNames.end(); it++) {
        if (it->second == id)
            return it->first;
    }
    return "";
}

string skindesignerapi::cTokenContainer::GetLoopTokenName(int loop, int id) {
    for (map<string, int>::iterator it = loopTokenNames[loop].begin(); it != loopTokenNames[loop].end(); it++) {
        if (it->second == id)
            return it->first;
    }
    return "";    
}

//Get name of loop from a loop token name
string skindesignerapi::cTokenContainer::LoopName(string &loopToken) {
    size_t hit = loopToken.find('{');
    if (hit != 0)
        return "";
    hit = loopToken.find('[');
    if (hit == string::npos)
        return "";
    return loopToken.substr(1, hit-1);
}
