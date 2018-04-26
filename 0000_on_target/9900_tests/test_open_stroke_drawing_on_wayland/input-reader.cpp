/* ************************************************************************
* Copyright (C) Continental AG and subsidiaries
* All rights reserved
*
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
* ************************************************************************/
/**
@file     input-reader.cpp
@brief    This file is to implement the input reader
@par      Project: GWM_MY18_HMI_CHB131
@author   Wei Zhaorong
*/

#include "input-reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <QDebug>
#include <QtCore/qcoreevent.h>

#define MAX_TOUCHSCREEN_Y 1280
#define MAX_RESOLUTION_Y 720

#define LOG_INFO(...)
#define LOG_DEBUG(...)

InputReader::InputReader(QString inputDevice) :
    QObject(),
    m_inputDevice(inputDevice),
    minX(0),
    minY(0),
    maxX(1280),
    maxY(720),
    m_isReaderEnabled(false)
{
    LOG_INFO("InputReader");
    connect(this, &InputReader::sgnStartReading,
            this, &InputReader::startReading,
            Qt::QueuedConnection);
}

InputReader::~InputReader()
{
    LOG_INFO("~InputReader");
}

void InputReader::startReading()
{
    LOG_INFO("InputReader|startReading");

    input_event iE;

    int lastX = 0;
    int lastY = 0;
    QString lastEventType = "M";

    bool isPressed = false;
    bool isPressValid = false;
    int fileDescriptor;

    if((fileDescriptor = open(m_inputDevice.toLocal8Bit(), O_RDONLY)) == -1)
    {
        LOG_INFO("InputReader|", "Device open ERROR");
    }

    if(fileDescriptor != -1)
    {
        while (getIsReaderEnabled() == true)
        {
            if (read(fileDescriptor, &iE, sizeof(struct input_event)) != -1)
            {
                LOG_DEBUG("InputReader|type:", iE.type,"|code:", iE.code, "|value:", iE.value, "|lastX:", lastX, "|lastY:", lastY,"|isPressed:", isPressed);

                switch(iE.type)
                {
                case EV_ABS: // mouse event happen
                {
                    if(iE.code == ABS_MT_POSITION_X) // X coordinate
                    {
                        lastX = iE.value;
                    }
                    else if (iE.code == ABS_MT_POSITION_Y) // Y coordinate
                    {
                        // magic here:
                        // the substraction (MAX_TOUCHSCREEN_Y - iE.value) is necessary because the resistive Y axis
                        // is 800 "pixels" wide AND reversed! (the point of origin for Y is on the lower side of the screen)
                        //lastY = (iE.value * MAX_RESOLUTION_Y) / MAX_TOUCHSCREEN_Y;
                        lastY = iE.value;
                    }
                    else
                    {
                        LOG_DEBUG("InputReader|", "Something is wrong with the input_event codes for EV_ABS");
                    }
                    break;
                }
                case EV_KEY:
                {
                    if(iE.code == BTN_TOUCH || iE.code == BTN_LEFT) // BTN_TOUCH - press or release touch screen
                    {
                        lastEventType = (iE.value == 0 ? "R" : "P");

                        if(iE.value == 1)
                        {
                            printf("%s(): %u\n", __FUNCTION__, __LINE__);
                            isPressed = true;
                            LOG_INFO("InputReader|", "set isPressed as true");
                        }
                    }
//#ifndef WL_EGL_ARM_PLATFORM
                    if ( iE.code == BTN_RIGHT )
                    {
                        printf("%s(): %u\n", __FUNCTION__, __LINE__);
                        lastX = minX;
                        lastY = minY;
                    }
//#endif
                    break;
                }
                case EV_REL:
                {
                    // the events here are relative the to last cursor position
                    if(iE.code == REL_X) // X coordinate
                    {
                        printf("%s(): %u\n", __FUNCTION__, __LINE__);
                        lastX += iE.value;
                    }
                    else if (iE.code == REL_Y) // Y coordinate
                    {
                        printf("%s(): %u\n", __FUNCTION__, __LINE__);
                        lastY += iE.value;
                    }
                    break;
                }
                case EV_SYN:
                {
                    if (isPressed && lastEventType == "P") 
                    {
                        if(((lastX > minX) && (lastX < maxX)) &&
                                ((lastY > minY) && (lastY < maxY)))
                        {
                            isPressValid = true;
                        }
                    }

                    if(isPressed && isPressValid)
                    {
                        if(((lastX > minX) && (lastX < maxX)) &&
                                ((lastY > minY) && (lastY < maxY)))
                        {
                            if(!(m_lastX == lastX && m_lastY == lastY))
                            {
                                emit sgnReadEvent(lastEventType, lastX, lastY, m_inputDevice);
                                m_lastX = lastX;
                                m_lastY = lastY;
                                LOG_DEBUG("InputReader|", "sgnReadEvent 1");
                            }

                            if(lastEventType == "R")
                            {
                                emit sgnReadEvent(lastEventType, lastX, lastY, m_inputDevice);
                                isPressed = false;
                                isPressValid = false;
                                LOG_DEBUG("InputReader|", "sgnReadEvent 2");
                            }
                        }
                        else {
                            emit sgnFakeRelease("R", m_lastX, m_lastY, m_inputDevice);
                            LOG_DEBUG("InputReader|", "sgnReadEvent 3");
                            isPressed = false;
                            isPressValid = false;
                        }
                    }

                    lastEventType = "M";
                    break;
                }
                }

            }
        }
        close(fileDescriptor);
        LOG_INFO("InputReader|", "InputReader close file");
    }
    else
    {
        LOG_INFO("InputReader|", "DirectReading: Invalid file descriptor");
    }
}

void InputReader::setArea(int x, int y, int width, int height)
{
    minX = x;
    minY = y;
    maxX = minX + width;
    maxY = minY + height;
    LOG_INFO("InputReader|setArea", "|minX:", minX, "|minY:", minY, "|maxX:", maxX, "|maxY:", maxY);
}

void InputReader::enableReader ()
{
    mutex.lock();
    if (m_isReaderEnabled == false)
    {
        //Do sgnStartReading() only when we need to change from false to true
        m_isReaderEnabled = true;
        emit sgnStartReading();
    }
    mutex.unlock();

    LOG_INFO("InputReader|enableReader", "enabled:", m_isReaderEnabled);
}

void InputReader::disableReader ()
{
    LOG_INFO("InputReader|disableReader");
    mutex.lock();
    m_isReaderEnabled = false;
    mutex.unlock();
}


bool InputReader::getIsReaderEnabled()
{
    LOG_DEBUG("InputReader|getIsReaderEnabled", "enabled:", m_isReaderEnabled);
    mutex.lock();
    bool isReaderEnabled = m_isReaderEnabled;
    mutex.unlock();
    return isReaderEnabled;
}
