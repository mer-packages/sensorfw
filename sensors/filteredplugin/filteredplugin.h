#ifndef FILTEREDPLUGIN_H
#define FILTEREDPLUGIN_H

#include "plugin.h"

class FilteredPlugin : public Plugin
{
private:
    void Register(class Loader& l);
    QStringList Dependencies();
};

#endif
