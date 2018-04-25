import QtQuick 2.0

Item {
    Text {
        id: simsumText
        
        font.family: "宋体"
        font.pixelSize: 30
        
        color : "red"
        
        text : "font test （文本测试）（宋体）"
    }

    Text {
        id: simkaiText
        
        y: simsumText.height
        
        font.family: "楷体"
        font.pixelSize: 30
        
        color : "red"
        
        text : "font test （文本测试）（楷体）"
    }
}
