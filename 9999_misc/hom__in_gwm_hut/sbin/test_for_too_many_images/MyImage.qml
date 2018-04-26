import QtQuick 2.0

Image {
    id: imageMain;

    property string baseName: "unknown";
    property int currentIndex: 0;

    source: {
        var ret = baseName;
        var index;

        index = currentIndex % 7;

        switch (index) {
        case 0:
            ret += "_normal";
            break;
        case 1:
            ret += "_pressed";
            break;
        case 2:
            ret += "_focus";
            break;
        case 3:
            ret += "_selected";
            break;
        case 4:
            ret += "_selectedfocus";
            break;
        case 5:
            ret += "_disfocus";
            break;
        case 6:
            ret += "_disable";
            break;
        }

        ret += ".png";

        if (!tools.fileExists(ret)) {
            ret = baseName + "_normal.png";
        }

        return ret;
    }

    Rectangle {
        id: background;

        z: -1;
        anchors.fill: parent;
        color: "black";
    }

    MouseArea {
        id: mouseArea;
        anchors.fill: parent;

        onClicked: {
            imageMain.currentIndex++;
        }
    }
}
