#pragma once

#include <string>
#include <vector>

#include <vdr/plugin.h>
#include "flat.h"

#define PLUGINCONFIGPATH (cPlugin::ConfigDirectory(PLUGIN_NAME_I18N))
#define PLUGINRESOURCEPATH (cPlugin::ResourceDirectory(PLUGIN_NAME_I18N))

class cFlatConfig
{
    private:
        cString checkSlashAtEnd(std::string path);
    public:
        cFlatConfig(void);
        ~cFlatConfig(void);

        void Init(void);
        void SetLogoPath(cString path);
    public:
        cString logoPath;
        cString iconPath;
};
