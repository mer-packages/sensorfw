/**
   @file parser.h
   @brief Sensord initiation point

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Lihan Guo <ext-lihan.guo@nokia.com>

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

#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include "logging.h"

class Parser
{
public:

    Parser(QStringList arguments);
    ~Parser();

    bool printHelp() const;
    QtMsgType getLogLevel() const;

    bool configFileInput() const;
    const QString& configFilePath() const;
    bool configDirInput() const;
    const QString& configDirPath() const;

    bool contextInfo() const;
    bool magnetometerCalibration() const;
    bool createDaemon() const;

private:
    void parsingCommandLine(QStringList arguments);

    bool printHelp_;
    bool contextInfo_;
    bool configFile_;
    bool configDir_;
    bool daemon_;
    bool magnetometerCalibration_;

    QString configFilePath_;
    QString configDirPath_;
    QtMsgType logLevel_;
};

#endif // PARSER_H
