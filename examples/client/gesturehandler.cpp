#include "gesturehandler.h"

GestureHandler::GestureHandler(int yThreshold, int zThreshold)
    : activated_(false)
    , validHist_(false)
    , yThreshold_(yThreshold)
    , zThreshold_(zThreshold)
    , prevYValue_(0)
    , prevZValue_(0)
{
}

void GestureHandler::handleData(const Key& data)
{
    switch (data.key())
    {
        case 199 :
            qDebug() << "Gesture detection activated";
            emit gestureDetectionActivated();
            activated_ = true;
        break;
        case 200 :
            qDebug() << "Gesture detection de-activated";
            emit gestureDetectionDeactiviated();
            activated_ = validHist_ = false;
        break;
        default:
            emit keyPressed(data.key());

            /*
            qDebug() << "Key SensorData ="
                     << data.KeyData().timestamp_
                     << (char)data.key() << "(" << data.key() << ")"
                     << data.KeyData().value_.pressed
                     << data.KeyData().value_.released;
            */
        break;
    }
}

void GestureHandler::handleData(const XYZ& value)
{
    if (activated_)
    {
        /*
        qDebug() << "XYZ SensorData ="
                 << value.XYZData().timestamp_
                 << value.XYZData().x_
                 << value.XYZData().y_
                 << value.XYZData().z_;
         */

        if (validHist_)
        {
            int yDiff = prevYValue_ - value.y();
            if ( yDiff > yThreshold_ )
            {
                qDebug() << "Increase volume (Y):" << yDiff;
                emit volumeIncreased();
            }
            else if ( -yDiff > yThreshold_ )
            {
                qDebug() << "Decrease volume (Y):" << yDiff;
                emit volumeDecreased();
            }

            int zDiff = prevZValue_ - value.z();
            if ( zDiff > zThreshold_ )
            {
                qDebug() << "Increase zoom (Z):" << zDiff;
                emit zoomIncreased();
            }
            else if ( -zDiff > zThreshold_ )
            {
                qDebug() << "Decrease zoom (Z):" << zDiff;
                emit zoomDecreased();
            }
        }
        else
        {
            qDebug() << "Discarding initial value...";
            validHist_ = true;
        }

        prevYValue_ = value.y();
        prevZValue_ = value.z();
    }
}
