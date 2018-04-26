import QtQuick 2.0
import QtQuick.Window 2.0

Item {
    id: root;

    Rectangle {
        id: rect1;
        property int i: 1;

        x: 0;
        y: 0;
        width: 200;
        height: 200;
        z: 100;
        
        color: "red"

        MouseArea {           
            anchors.fill: parent;

            onClicked: {
                console.log("mouseArea1.onClicked");
                parent.parent = null;
                parent.parent = root;
            }
        }
    }
    
    Rectangle {
        id: rect2;
        property int i: 1;

        x: 100;
        y: 100;
        width: 200;
        height: 200;
        z: 100;
        
        color: "green"

        MouseArea {           
            anchors.fill: parent;

            onClicked: {
                console.log("mouseArea2.onClicked");
                parent.parent = null;
                parent.parent = root;
            }
        }
    }
}
