#ifndef GESTUREHANDLER_H
#define GESTUREHANDLER_H

#include <QtDebug>

#include "xyz.h"
#include "key.h"

class GestureHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int yThreshold READ yThreshold WRITE setYThreshold)
    Q_PROPERTY(int zThreshold READ zThreshold WRITE setZThreshold)

public:
    GestureHandler(int yThreshold = DEFAULT_Y_THRESHOLD, int zThreshold = DEFAULT_Z_THRESHOLD);
    virtual ~GestureHandler() {}

    int yThreshold() const { return yThreshold_; }
    void setYThreshold(int value) { yThreshold_ = value; }

    int zThreshold() const { return zThreshold_; }
    void setZThreshold(int value) { zThreshold_ = value; }

public slots:
    void handleData(const Key& data);
    void handleData(const XYZ& value);

signals:
    void gestureDetectionActivated();
    void gestureDetectionDeactiviated();

    void keyPressed(int keyCode);

    void volumeIncreased();
    void volumeDecreased();

    void zoomIncreased();
    void zoomDecreased();

private:
    static const int DEFAULT_Y_THRESHOLD = 10; // ZOOM
    static const int DEFAULT_Z_THRESHOLD = 10; // VOLUME

    bool activated_;
    bool validHist_;

    int  yThreshold_;
    int  zThreshold_;

    int  prevYValue_;
    int  prevZValue_;
};

#endif // GESTUREHANDLER_H



