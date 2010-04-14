#ifndef ARIANEPLUGIN_H
#define ARIANEPLUGIN_H

#include "plugin.h"

class ArianePlugin : public Plugin
{
private:
    void Register(class Loader& l);
    QStringList Dependencies();
};

#endif
