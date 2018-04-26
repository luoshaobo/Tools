/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQuick 2.0

Item {
    width: 600;
    height: 480;

    ListView {
        id: listView;

        highlightFollowsCurrentItem: false;

        width: 600;
        height: 300;

        cacheBuffer: listView.height / 5 * 2;

        model: myModel
        delegate: Text {
            height: listView.height / 5;
            width: listView.width;
            text: "Animal: " + type + ", " + size;
            horizontalAlignment: Text.AlignLeft;
            verticalAlignment: Text.AlignVCenter;

            Rectangle {
                anchors.fill: parent;
                color: listView.currentIndex === model.index ? "red" : "white";
                z: -1;
            }
        }

        onCurrentIndexChanged: {
            myModel.currentIndex = currentIndex;
        }

        currentIndex: 46;
    }

    Rectangle {
        id: button1;
        x: 0;
        y: listView.y + listView.height;
        width: 600;
        height: 50;

        color: "pink";

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                listView.currentIndex = 46;
            }
        }

        Text {
            anchors.fill: parent;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            text: "set currentIndex=46";
        }
    }

    Rectangle {
        id: button2;
        x: 0;
        y: listView.y + listView.height + button1.height;
        width: 600;
        height: 50;

        color: "green";

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                listView.currentIndex++;
            }
        }

        Text {
            anchors.fill: parent;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            text: "currentIndex++";
        }
    }

    Rectangle {
        id: button3;
        x: 0;
        y: listView.y + listView.height + button1.height + button2.height;
        width: 600;
        height: 50;

        color: "blue";

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                listView.currentIndex--;
            }
        }

        Text {
            anchors.fill: parent;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            text: "currentIndex--";
        }
    }
}
