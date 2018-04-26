import QtQuick 2.0

Rectangle {
    x: 0;
    y: 0;
    width: 1280;
    height: 720;

    color: "pink";
    
    Rectangle {
        id: indicator;

        x: 100;
        y: 100;
        width: 100;
        height: 100;

        color: "grey";

        MouseArea {
            anchors.fill: parent;
            acceptedButtons: Qt.LeftButton;
            
            onClicked: {
                console.log("### clicked in qml");
                globalConfig.bgColor = 1;
                var color = globalConfig.bgColor;
                var w = globalConfig.screenWidth;
                globalConfig.testOp001();
            }
            
            Connections {
                target : globalConfig
                onBgColorChanged: {
                    console.info("### the signal onBgColorChanged() is received in QML!");
                }
            }
        }
    }
}
