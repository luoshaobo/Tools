import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    visible: true;
    x: 0;
    y: 0;
    width: 800;
    height: 480;
    title: "test_screen";
    flags: Qt.FramelessWindowHint;
    color: "#00000000";

    Rectangle {
        id: rootRect;
        property int shift: 0;

        x: 0;
        y: 0;
        width: parent == null ? 0 : parent.width / 2;
        height: parent == null ? 0 : parent.height / 2;

        color: "green";

        Connections {
            target: rootRect;
            onShiftChanged: {
                console.log("rootRect.onShiftChanged: shift=" + rootRect.shift);
            }
        }

        Binding {
            target: rootRect;
            property: "x";
            value: rootRect.shift;
        }

        MouseArea {
            id: mouseArea;
            
            anchors.fill: parent;

            onClicked: {
                console.log("mouseArea.onClicked: shift=" + rootRect.shift);
                console.log("mouseArea.onClicked: rootRect.x += 5");
                rootRect.x += 5;
                console.log("mouseArea.onClicked: shift=" + rootRect.shift);
                console.log("")
            }
            
            onDoubleClicked: {
                console.log("mouseArea.onDoubleClicked: shift=" + rootRect.shift);
                console.log("mouseArea.onClicked: rootRect.shift = 0");
                rootRect.shift = 0;
                console.log("mouseArea.onDoubleClicked: shift=" + rootRect.shift);
                rootRect.shiftChanged();        // to send property changed signal
                console.log("mouseArea.onDoubleClicked: shift=" + rootRect.shift);
                console.log("")
            }
        }
    }
}