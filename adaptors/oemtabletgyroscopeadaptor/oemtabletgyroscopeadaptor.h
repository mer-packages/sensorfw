#ifndef OEMTABLETGYROSCOPEADAPTOR_H
#define OEMTABLETGYROSCOPEADAPTOR_H

#include "sysfsadaptor.h"
#include "datatypes/orientationdata.h"

class OEMTabletGyroscopeAdaptor : public SysfsAdaptor
{
    Q_OBJECT;
public:
    /**
     * Factory method for gaining a new instance of this adaptor class.
     *
     * @param id Identifier for the adaptor.
     * @return A pointer to created adaptor with base class type.
     */
    static DeviceAdaptor* factoryMethod(const QString& id)
    {
        return new OEMTabletGyroscopeAdaptor(id);
    }

protected:

    /**
     * Constructor. Protected to force externals to use factory method.
     *
     * @param id Identifier for the adaptor.
     */
    OEMTabletGyroscopeAdaptor(const QString& id);

    /**
     * Destructor.
     */
    ~OEMTabletGyroscopeAdaptor();

private:

    /**
     * Read and process data. Run when sysfsadaptor has detected new
     * available data.
     *
     * @param pathId PathId for the file that had event.
     * @param fd Open file descriptor with new data. See
     *           #SysfsAdaptor::processSample()
     */
    void processSample(int pathId, int fd);

    DeviceAdaptorRingBuffer<TimedXyzData>* gyroscopeBuffer_;
};

#endif
