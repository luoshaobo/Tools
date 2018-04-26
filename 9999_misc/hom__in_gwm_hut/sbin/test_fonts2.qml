import QtQuick 2.0

Item {
    property string textColor: "white"

    Text {
        id: txt1
        
        font.family: "Eurostile-CH_forGW"
        font.pixelSize: 32
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（Eurostile-CH_forGW）pixel size: 32"
    }

    Text {
        id: txt2
        
        y: txt1.y + txt1.height
        
        font.family: "方正兰亭黑Pro_GB18030"
        font.pixelSize: 26
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（Eurostile-CH_forGW）pixel size: 26"
    }
    
    Text {
        id: txt3
        
        y: txt2.y + txt2.height
        
        font.family: "Eurostile LT Std Ext Two"
        font.pixelSize: 32
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（Eurostile LT Std Ext Two）pixel size: 32"
    }

    Text {
        id: txt4
        
        y: txt3.y + txt3.height
        
        font.family: "Eurostile"
        font.pixelSize: 26
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（Eurostile LT Std Ext Two）pixel size: 26"
    }
    
    Text {
        id: txt5
        
        y: txt4.y + txt4.height
        
        font.family: "宋体"
        font.pixelSize: 32
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（宋体）pixel size: 32"
    }

    Text {
        id: txt6
        
        y: txt5.y + txt5.height
        
        font.family: "宋体"
        font.pixelSize: 26
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（宋体）pixel size: 26"
    }
    
    Text {
        id: txt7
        
        y: txt6.y + txt6.height
        
        font.family: "楷体"
        font.pixelSize: 32
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（楷体）pixel size: 32"
    }

    Text {
        id: txt8
        
        y: txt7.y + txt7.height
        
        font.family: "楷体"
        font.pixelSize: 26
        
        color : textColor
        
        text : "999 FONT TEST font test （文本测试）（楷体）pixel size: 26"
    }
}
