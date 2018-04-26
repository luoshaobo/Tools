import QtQuick 2.0

Rectangle {
    anchors.fill: parent;
    color: "black";
    Text {
        color: "#FF440000";
        text: "123"
        font.pointSize : 20;
    }


    Rectangle {
        x: 0;
        y: 0;
        width: 300;
        height: 300;

        opacity: 0.5;

        //color: "#88000000";
        color: "#44000000";
    }

    Rectangle {
        x: 0;
        y: 0;
        width: 300;
        height: 300;

        opacity: 0.2;
        color: "#FF000000";
    }


}
