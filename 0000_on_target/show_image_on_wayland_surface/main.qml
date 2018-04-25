import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    width: 1280
    height: 720
    title: qsTr("show_image_on_wayland_surface")
    flags: Qt.FramelessWindowHint
    color: "#00000000"

    Rectangle {
        anchors.fill: parent;
        color: "#00000000"
    }

    Image {
        anchors.fill: parent;
        fillMode: controller.fillMode == 0 ? Image.Stretch :
                  controller.fillMode == 1 ? Image.PreserveAspectFit :
                  controller.fillMode == 2 ? Image.PreserveAspectCrop :
                  controller.fillMode == 3 ? Image.Tile :
                  controller.fillMode == 4 ? Image.TileVertically :
                  controller.fillMode == 5 ? Image.TileHorizontally :
                  controller.fillMode == 6 ? Image.Pad :
                  Image.Pad;

        horizontalAlignment : Image.AlignLeft;
        verticalAlignment : Image.AlignTop;

        source: "file:///" + controller.imageFilePath;
    }
}
