import QtQuick 2.0

Rectangle {
    id: rootRect;

    property string imageBaseDir: "/media/datastore/ui/resources/CYC/assets/images/ambiance_1";

    x: 0;
    y: 0;
    width: parent == null ? 0 : parent.width;
    height: parent == null ? 0 : parent.height;

    color: "black";

    Flickable {
        anchors.fill: parent;
        contentWidth: width;
        contentHeight: 100000;

        MyImage {
            id: image_0;
        }

