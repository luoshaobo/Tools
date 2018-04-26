import QtQuick 2.2

Rectangle {
    x: 0
    y: 0
    width: 800
    height:480

    color: Qt.rgba(0, 255, 0, 255);

    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("=================== haha");
        }
    }
}
