/**
   @file datafaker.h
   @brief Data synthesizing utility
   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
*/

#include <QObject>
#include <QMap>
#include <QString>

/**
 * @brief Helper class for synthesizing input events to file handles.
 * 
 * 
 */
class DataFaker : public QObject {
    Q_OBJECT;
public:
    DataFaker(QObject* parent = 0);
    ~DataFaker();

    /**
     * Opens a filehandle and associates with the given name.
     * 
     * @param path File to open.
     * @param name Name to associate as a key to newly opened fd.
     * @return \c true on success, \c false on failure.
     */
    bool open(QString path, QString name);

    /**
     * Closes the filehandle associated with name.
     * 
     * @param name Name for the filehandle to close.
     * @return \c true on success, \c false on failure.
     */
    bool close(QString name);
    
    /**
     * Write chunk of data to filehandle associated with name.
     * @param name Name of the fd.
     * @param data Pointer to buffer.
     * @param size Number of bytes to write from buffer.
     * @return \c true on success, \c false on failure.
     */
    bool write(QString name, const void *data, int size);

    /**
     * Wrapper for writing input events.
     * 
     * @param name Name of the fd.
     * @param type Input event type.
     * @param code Input event code.
     * @param value Input event value.
     * @return \c true on success, \c false on failure.
     */
    bool write(QString name, int type, int code, int value);

public slots:
    void cleanup();

private:
    QMap<QString, int> m_fdMap;
};
