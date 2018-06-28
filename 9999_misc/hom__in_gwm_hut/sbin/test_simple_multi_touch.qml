import QtQuick 2.0

Rectangle {
    width: 1280
    height: 720
    
    MultiPointTouchArea {
        anchors.fill: parent
        touchPoints: [
            TouchPoint { id: point1 },
            TouchPoint { id: point2 }
        ]
    }

    Rectangle {
        width: 30; height: 30
        color: "green"
        x: point1.x
        y: point1.y
    }

    Rectangle {
        width: 30; height: 30
        color: "yellow"
        x: point2.x
        y: point2.y
    }
}
