#include "config.h"

cFlatConfig::cFlatConfig(void) {
    logoPath = "";
    iconPath = "";
}

cFlatConfig::~cFlatConfig(void) {
}


void cFlatConfig::Init(void) {
    if( !strcmp(logoPath, "") )
        logoPath = cString::sprintf("%s/logos/", PLUGINRESOURCEPATH);
    dsyslog("skinflat: logopath: %s", *logoPath);
    if( !strcmp(iconPath, "") )
        iconPath = cString::sprintf("%s/icons/", PLUGINRESOURCEPATH);
    dsyslog("skinflat: iconPath: %s", *iconPath);
}

void cFlatConfig::SetLogoPath(cString path) {
    logoPath = checkSlashAtEnd(*path);
}

cString cFlatConfig::checkSlashAtEnd(std::string path) {
    try {
        if (!(path.at(path.size()-1) == '/'))
            return cString::sprintf("%s/", path.c_str());
    } catch (...) {return path.c_str();}
    return path.c_str();
}
