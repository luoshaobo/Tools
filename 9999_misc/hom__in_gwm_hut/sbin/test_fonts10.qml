import QtQuick 2.0

Item {
    property string textColor: "white"

    Text {
        id: txt1
        
        font.family: "Eurostile-CH_forGW Test"
        font.pixelSize: 32
        
        color : textColor
        
        text : "（文本测试）（Eurostile-CH_forGW）pixel size: 32 (999 FONT TEST font test)\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "0123456789\n"
    }

    Text {
        id: txt2
        
        y: txt1.y + txt1.height
        
        font.family: "Eurostile-CH_forGW Test"
        font.pixelSize: 26
        
        color : textColor
        
        text : "（文本测试）（Eurostile-CH_forGW）pixel size: 26 (999 FONT TEST font test)\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "0123456789\n"
    }
}

