/* ************************************************************************
* Copyright (C) Continental Automotive GmbH 2016
* All rights reserved
*
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
* ************************************************************************/

/**
@file     input-reader.h
@brief    This file is to defination the input reader
@par      Project: GWM_MY18_HMI_CHB131
@author   Wei Zhaorong
*/

#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <QObject>
//#include <logger.h>
#include <QMutex>

class InputReader : public QObject
{
    Q_OBJECT
public:
    explicit InputReader (QString inputDevice);
    ~InputReader ();
    void startReading ();
    void cmd_startReading();

signals:
    void sgnReadEvent (QString type, int x, int y, QString inputDevice);
    void sgnFakeRelease(QString type, int x, int y, QString inputDevice);
    void sgnStartReading ();

public slots:
    void setArea (int x, int y, int width, int height);
    void enableReader ();
    void disableReader ();

private:
    bool getIsReaderEnabled ();

    QString m_inputDevice;

    int minX;
    int minY;
    int maxX;
    int maxY;
    int m_lastX;
    int m_lastY;

    QMutex mutex;

    bool m_isReaderEnabled;
};

#endif // INPUTREADER_H
