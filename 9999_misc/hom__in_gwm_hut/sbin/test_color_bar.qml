import QtQuick 2.0

Rectangle {
    x: 0;
    y: 0;
    width: 1280;
    height: 720;

    color: "black";

    Rectangle {
        x: 0;
        y: 200;
        width: 1280;
        height: 50;

        color: "#00000000";

        Text {
            id: currentValue;

            property string valueStr: colorBarMouseArea.positionStr + " " + colorBarMouseArea.pressedStateStr;

            anchors.fill: parent;
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;

            font.pixelSize: 20;
            color: "red";
            text: valueStr;
        }
    }

    Rectangle {
        x: 320;
        y: 400;
        width: 640;
        height: 50;

        color: "lightgreen";

        Rectangle {
            id: indicator;

            x: 0;
            y: -5;
            width: 2;
            height: 60;

            visible: (x > 0 && x < parent.width);
            color: "red";

            onXChanged: {
                console.log("indicator.onXChanged(): x=", x);
            }
        }

        MouseArea {
            id: colorBarMouseArea;

            property string positionStr: "";
            property string pressedStateStr: "";

            anchors.fill: parent;
            acceptedButtons: Qt.LeftButton;

            onEntered: {
                console.log("colorBarMouseArea.onEntered()");
            }

            onPositionChanged: {
                console.log("colorBarMouseArea.onPositionChanged(): x=", mouse.x, "y=", mouse.y);
                setCurrentPositionStr(mouse);
            }

            onExited: {
                console.log("colorBarMouseArea.onExited()");
            }

            onPressed: {
                console.log("colorBarMouseArea.onPressed(): x=", mouse.x, "y=", mouse.y);
                setCurrentPositionStr(mouse);
                setCurrentPressedStateStr(true);
            }

            onReleased: {
                console.log("colorBarMouseArea.onReleased(): x=", mouse.x, "y=", mouse.y);
                setCurrentPositionStr(mouse);
                setCurrentPressedStateStr(false);
            }

            function setCurrentPositionStr(mouse) {
                positionStr = ("(x,y)=(" + mouse.x + "," + mouse.y + ")");
                indicator.x = (parseInt(mouse.x / 10) * 10);
            }

            function setCurrentPressedStateStr(pressed) {
                pressedStateStr = (pressed ? "pressed" : "released");
            }
        }
    }

    Rectangle {
        x: 100;
        y: 100;
        width: 40;
        height: 40;

        color: "red";
    }

    Rectangle {
        x: 200;
        y: 100;
        width: 50;
        height: 50;

        color: "red";
    }
}
