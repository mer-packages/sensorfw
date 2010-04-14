#ifndef RAWORIENTATIONPLUGIN_H
#define RAWORIENTATIONPLUGIN_H

#include "plugin.h"

class RawOrientationPlugin : public Plugin
{
private:
    void Register(class Loader& l);
    QStringList Dependencies();
};

#endif
