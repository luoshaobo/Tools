import QtQuick 2.0

Item {
    property string textColor: "white"

    Text {
        id: txt1
        
        font.family: "Eurostile-CH_forGW"
        font.pixelSize: 32
        
        color : textColor
        
        text : "(999 FONT TEST font test)[] （文本测试）（Eurostile-CH_forGW）pixel size: 32\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "qwertyuiopasdfghjklzxcvbnm\n"
            + "Gas Station  Hotel  Food  Bus Station  Bank  Sights\n"
    }

    Text {
        id: txt2
        
        y: txt1.y + txt1.height
        
        font.family: "Eurostile-CH_forGW"
        font.pixelSize: 26
        
        color : textColor
        
        text : "(999 FONT TEST font test)[] （文本测试）（Eurostile-CH_forGW）pixel size: 26\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "qwertyuiopasdfghjklzxcvbnm\n"
            + "Gas Station  Hotel  Food  Bus Station  Bank  Sights\n"
    }
    
    Text {
        id: txt3
        
        y: txt2.y + txt2.height
        
        font.family: "Eurostile-CH_forGW Test"
        font.pixelSize: 32
        
        color : textColor
        
        text : "(999 FONT TEST font test)[] （文本测试）（Eurostile-CH_forGW Test）pixel size: 32\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "qwertyuiopasdfghjklzxcvbnm\n"
            + "Gas Station  Hotel  Food  Bus Station  Bank  Sights\n"
    }

    Text {
        id: txt4
        
        y: txt3.y + txt3.height
        
        font.family: "Eurostile-CH_forGW Test"
        font.pixelSize: 26
        
        color : textColor
        
        text : "(999 FONT TEST font test)[] （文本测试）（Eurostile-CH_forGW Test）pixel size: 26\n"
            + "(Fg9OPLlI)[Fg9OPLlI]{Fg9OPLlI} ~`!@#$%^&*()[]{}|\;:\'\",.<>/?\n"
            + "qwertyuiopasdfghjklzxcvbnm\n"
            + "Gas Station  Hotel  Food  Bus Station  Bank  Sights\n"
    }
}
