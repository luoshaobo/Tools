import QtQuick 2.0

Item {
    Text {
        id: simsumText
        
        font.family: "宋体"
        font.pixelSize: 32
        
        color : "red"
        
        text : "999 font test （文本测试）（宋体）"
    }

    Text {
        id: simkaiText
        
        y: simsumText.height
        
        font.family: "楷体"
        font.pixelSize: 26
        
        color : "red"
        
        text : "999 font test （文本测试）（楷体）"
    }
}
