import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    visible: true;
    x: controller.windowX;
    y: controller.windowY;
    width: controller.windowWidth;
    height: controller.windowHeight;
    title: controller.windowTitle;
    flags: controller.windowFrameless ? Qt.FramelessWindowHint : Qt.Window;
    color: controller.windowBgColor;
}
