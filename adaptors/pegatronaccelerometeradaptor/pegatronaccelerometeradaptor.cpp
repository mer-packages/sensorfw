
#include "pegatronaccelerometeradaptor.h"

#include "logging.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <QMap>

#include "datatypes/utils.h"

#define DEVICE_MATCH_STRING "accelerometer"

PegatronAccelerometerAdaptor::PegatronAccelerometerAdaptor(const QString& id) :
    InputDevAdaptor(id, 1)
{
    //This was previously in the base class, but it's not
    //possible call virtual methods from base class constructor.
    //TODO: find a way to get rid of all child classes calling this
    //manually.

    sleep(5);

    if (!getInputDevices(DEVICE_MATCH_STRING)) {
        sensordLogW() << "Input device not found.";
    }

    accelerometerBuffer_ = new DeviceAdaptorRingBuffer<OrientationData>(128);
    setAdaptedSensor("accelerometer", "Internal accelerometer coordinates", accelerometerBuffer_);

    // Set Metadata
    setDescription("Input device accelerometer adaptor (Pegtron Lucid Tablet)");
    introduceAvailableDataRange(DataRange(-512, 512, 1));
    introduceAvailableInterval(DataRange(0, 0, 0));
    introduceAvailableInterval(DataRange(50, 2000, 0)); // -> [1,100] Hz
    setDefaultInterval(300);
}

PegatronAccelerometerAdaptor::~PegatronAccelerometerAdaptor()
{
    delete accelerometerBuffer_;
}

void PegatronAccelerometerAdaptor::interpretEvent(int src, struct input_event *ev)
{
    Q_UNUSED(src);

    switch (ev->type) {
        case EV_ABS:
            switch (ev->code) {
                case ABS_X:
                    orientationValue_.x_ = ev->value;
                    break;
                case ABS_Y:
                    orientationValue_.y_ = ev->value;
                    break;
                case ABS_Z:
                    orientationValue_.z_ = ev->value;
                    break;
            }
            break;

    }
}

void PegatronAccelerometerAdaptor::interpretSync(int src, struct input_event *ev)
{
    Q_UNUSED(src);
    commitOutput(ev);
}

void PegatronAccelerometerAdaptor::commitOutput(struct input_event *ev)
{
    OrientationData* d = accelerometerBuffer_->nextSlot();

    d->timestamp_ = Utils::getTimeStamp(&(ev->time));
    d->x_ = orientationValue_.x_;
    d->y_ = orientationValue_.y_;
    d->z_ = orientationValue_.z_;

    accelerometerBuffer_->commit();
    accelerometerBuffer_->wakeUpReaders();
}

unsigned int PegatronAccelerometerAdaptor::evaluateIntervalRequests(int& sessionId) const
{
    unsigned int highestValue = 0;
    int winningSessionId = -1;

    if (m_intervalMap.size() == 0)
    {
        sessionId = winningSessionId;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it;
    it = m_intervalMap.begin();
    highestValue = it.value();
    winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it)
    {
        if ((it.value() < highestValue) && (it.value() > 0)) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}
