import QtQuick 2.0

Rectangle {
    width: 1280
    height: 720        
    
    Flickable {   
        anchors.fill: parent
        flickableDirection: Flickable.HorizontalFlick
        
        property real lastContentX: 0;
        
        onContentXChanged: {
            if (lastContentX > 0 && lastContentX < 100) {
                if (contentX >= 100) {
                    console.log("moving to left");
                }
            }
            
            if (lastContentX < 0 && lastContentX > -100) {
                if (contentX <= -100) {
                    console.log("moving to right");
                }
            }
            
            lastContentX = contentX;
        }
        
        Item {
            id: rootRect;

            x: 0;
            y: 0;
            width: 1280
            height: 720

            ListModel {
                id: colorListModel;
                ListElement { colorName: "aliceblue"; colorValue: "(240, 248, 255)"; }
                ListElement { colorName: "antiquewhite"; colorValue: "(250, 235, 215)"; }
                ListElement { colorName: "aqua"; colorValue: "( 0, 255, 255)"; }
                ListElement { colorName: "aquamarine"; colorValue: "(127, 255, 212)"; }
                ListElement { colorName: "azure"; colorValue: "(240, 255, 255)"; }
                ListElement { colorName: "beige"; colorValue: "(245, 245, 220)"; }
                ListElement { colorName: "bisque"; colorValue: "(255, 228, 196)"; }
                ListElement { colorName: "black"; colorValue: "( 0, 0, 0)"; }
                ListElement { colorName: "blanchedalmond"; colorValue: "(255, 235, 205)"; }
                ListElement { colorName: "blue"; colorValue: "( 0, 0, 255)"; }
                ListElement { colorName: "blueviolet"; colorValue: "(138, 43, 226)"; }
                ListElement { colorName: "brown"; colorValue: "(165, 42, 42)"; }
                ListElement { colorName: "burlywood"; colorValue: "(222, 184, 135)"; }
                ListElement { colorName: "cadetblue"; colorValue: "( 95, 158, 160)"; }
                ListElement { colorName: "chartreuse"; colorValue: "(127, 255, 0)"; }
                ListElement { colorName: "chocolate"; colorValue: "(210, 105, 30)"; }
                ListElement { colorName: "coral"; colorValue: "(255, 127, 80)"; }
                ListElement { colorName: "cornflowerblue"; colorValue: "(100, 149, 237)"; }
                ListElement { colorName: "cornsilk"; colorValue: "(255, 248, 220)"; }
                ListElement { colorName: "crimson"; colorValue: "(220, 20, 60)"; }
                ListElement { colorName: "cyan"; colorValue: "( 0, 255, 255)"; }
                ListElement { colorName: "darkblue"; colorValue: "( 0, 0, 139)"; }
                ListElement { colorName: "darkcyan"; colorValue: "( 0, 139, 139)"; }
                ListElement { colorName: "darkgoldenrod"; colorValue: "(184, 134, 11)"; }
                ListElement { colorName: "darkgray"; colorValue: "(169, 169, 169)"; }
                ListElement { colorName: "darkgreen"; colorValue: "( 0, 100, 0)"; }
                ListElement { colorName: "darkgrey"; colorValue: "(169, 169, 169)"; }
                ListElement { colorName: "darkkhaki"; colorValue: "(189, 183, 107)"; }
                ListElement { colorName: "darkmagenta"; colorValue: "(139, 0, 139)"; }
                ListElement { colorName: "darkolivegreen"; colorValue: "( 85, 107, 47)"; }
                ListElement { colorName: "darkorange"; colorValue: "(255, 140, 0)"; }
                ListElement { colorName: "darkorchid"; colorValue: "(153, 50, 204)"; }
                ListElement { colorName: "darkred"; colorValue: "(139, 0, 0)"; }
                ListElement { colorName: "darksalmon"; colorValue: "(233, 150, 122)"; }
                ListElement { colorName: "darkseagreen"; colorValue: "(143, 188, 143)"; }
                ListElement { colorName: "darkslateblue"; colorValue: "( 72, 61, 139)"; }
                ListElement { colorName: "darkslategray"; colorValue: "( 47, 79, 79)"; }
                ListElement { colorName: "darkslategrey"; colorValue: "( 47, 79, 79)"; }
                ListElement { colorName: "darkturquoise"; colorValue: "( 0, 206, 209)"; }
                ListElement { colorName: "darkviolet"; colorValue: "(148, 0, 211)"; }
                ListElement { colorName: "deeppink"; colorValue: "(255, 20, 147)"; }
                ListElement { colorName: "deepskyblue"; colorValue: "( 0, 191, 255)"; }
                ListElement { colorName: "dimgray"; colorValue: "(105, 105, 105)"; }
                ListElement { colorName: "dimgrey"; colorValue: "(105, 105, 105)"; }
                ListElement { colorName: "dodgerblue"; colorValue: "( 30, 144, 255)"; }
                ListElement { colorName: "firebrick"; colorValue: "(178, 34, 34)"; }
                ListElement { colorName: "floralwhite"; colorValue: "(255, 250, 240)"; }
                ListElement { colorName: "forestgreen"; colorValue: "( 34, 139, 34)"; }
                ListElement { colorName: "fuchsia"; colorValue: "(255, 0, 255)"; }
                ListElement { colorName: "gainsboro"; colorValue: "(220, 220, 220)"; }
                ListElement { colorName: "ghostwhite"; colorValue: "(248, 248, 255)"; }
                ListElement { colorName: "gold"; colorValue: "(255, 215, 0)"; }
                ListElement { colorName: "goldenrod"; colorValue: "(218, 165, 32)"; }
                ListElement { colorName: "gray"; colorValue: "(128, 128, 128)"; }
                ListElement { colorName: "grey"; colorValue: "(128, 128, 128)"; }
                ListElement { colorName: "green"; colorValue: "( 0, 128, 0)"; }
                ListElement { colorName: "greenyellow"; colorValue: "(173, 255, 47)"; }
                ListElement { colorName: "honeydew"; colorValue: "(240, 255, 240)"; }
                ListElement { colorName: "hotpink"; colorValue: "(255, 105, 180)"; }
                ListElement { colorName: "indianred"; colorValue: "(205, 92, 92)"; }
                ListElement { colorName: "indigo"; colorValue: "( 75, 0, 130)"; }
                ListElement { colorName: "ivory"; colorValue: "(255, 255, 240)"; }
                ListElement { colorName: "khaki"; colorValue: "(240, 230, 140)"; }
                ListElement { colorName: "lavender"; colorValue: "(230, 230, 250)"; }
                ListElement { colorName: "lavenderblush"; colorValue: "(255, 240, 245)"; }
                ListElement { colorName: "lawngreen"; colorValue: "(124, 252, 0)"; }
                ListElement { colorName: "lemonchiffon"; colorValue: "(255, 250, 205)"; }
                ListElement { colorName: "lightblue"; colorValue: "(173, 216, 230)"; }
                ListElement { colorName: "lightcoral"; colorValue: "(240, 128, 128)"; }
                ListElement { colorName: "lightcyan"; colorValue: "(224, 255, 255)"; }
                ListElement { colorName: "lightgoldenrodyellow"; colorValue: "(250, 250, 210)"; }
                ListElement { colorName: "lightgray"; colorValue: "(211, 211, 211)"; }
                ListElement { colorName: "lightgreen"; colorValue: "(144, 238, 144)"; }
                ListElement { colorName: "lightgrey"; colorValue: "(211, 211, 211)"; }
                ListElement { colorName: "lightpink"; colorValue: "(255, 182, 193)"; }
                ListElement { colorName: "lightsalmon"; colorValue: "(255, 160, 122)"; }
                ListElement { colorName: "lightseagreen"; colorValue: "( 32, 178, 170)"; }
                ListElement { colorName: "lightskyblue"; colorValue: "(135, 206, 250)"; }
                ListElement { colorName: "lightslategray"; colorValue: "(119, 136, 153)"; }
                ListElement { colorName: "lightslategrey"; colorValue: "(119, 136, 153)"; }
                ListElement { colorName: "lightsteelblue"; colorValue: "(176, 196, 222)"; }
                ListElement { colorName: "lightyellow"; colorValue: "(255, 255, 224)"; }
                ListElement { colorName: "lime"; colorValue: "( 0, 255, 0)"; }
                ListElement { colorName: "limegreen"; colorValue: "( 50, 205, 50)"; }
                ListElement { colorName: "linen"; colorValue: "(250, 240, 230)"; }
                ListElement { colorName: "magenta"; colorValue: "(255, 0, 255)"; }
                ListElement { colorName: "maroon"; colorValue: "(128, 0, 0)"; }
                ListElement { colorName: "mediumaquamarine"; colorValue: "(102, 205, 170)"; }
                ListElement { colorName: "mediumblue"; colorValue: "( 0, 0, 205)"; }
                ListElement { colorName: "mediumorchid"; colorValue: "(186, 85, 211)"; }
                ListElement { colorName: "mediumpurple"; colorValue: "(147, 112, 219)"; }
                ListElement { colorName: "mediumseagreen"; colorValue: "( 60, 179, 113)"; }
                ListElement { colorName: "mediumslateblue"; colorValue: "(123, 104, 238)"; }
                ListElement { colorName: "mediumspringgreen"; colorValue: "( 0, 250, 154)"; }
                ListElement { colorName: "mediumturquoise"; colorValue: "( 72, 209, 204)"; }
                ListElement { colorName: "mediumvioletred"; colorValue: "(199, 21, 133)"; }
                ListElement { colorName: "midnightblue"; colorValue: "( 25, 25, 112)"; }
                ListElement { colorName: "mintcream"; colorValue: "(245, 255, 250)"; }
                ListElement { colorName: "mistyrose"; colorValue: "(255, 228, 225)"; }
                ListElement { colorName: "moccasin"; colorValue: "(255, 228, 181)"; }
                ListElement { colorName: "navajowhite"; colorValue: "(255, 222, 173)"; }
                ListElement { colorName: "navy"; colorValue: "( 0, 0, 128)"; }
                ListElement { colorName: "oldlace"; colorValue: "(253, 245, 230)"; }
                ListElement { colorName: "olive"; colorValue: "(128, 128, 0)"; }
                ListElement { colorName: "olivedrab"; colorValue: "(107, 142, 35)"; }
                ListElement { colorName: "orange"; colorValue: "(255, 165, 0)"; }
                ListElement { colorName: "orangered"; colorValue: "(255, 69, 0)"; }
                ListElement { colorName: "orchid"; colorValue: "(218, 112, 214)"; }
                ListElement { colorName: "palegoldenrod"; colorValue: "(238, 232, 170)"; }
                ListElement { colorName: "palegreen"; colorValue: "(152, 251, 152)"; }
                ListElement { colorName: "paleturquoise"; colorValue: "(175, 238, 238)"; }
                ListElement { colorName: "palevioletred"; colorValue: "(219, 112, 147)"; }
                ListElement { colorName: "papayawhip"; colorValue: "(255, 239, 213)"; }
                ListElement { colorName: "peachpuff"; colorValue: "(255, 218, 185)"; }
                ListElement { colorName: "peru"; colorValue: "(205, 133, 63)"; }
                ListElement { colorName: "pink"; colorValue: "(255, 192, 203)"; }
                ListElement { colorName: "plum"; colorValue: "(221, 160, 221)"; }
                ListElement { colorName: "powderblue"; colorValue: "(176, 224, 230)"; }
                ListElement { colorName: "purple"; colorValue: "(128, 0, 128)"; }
                ListElement { colorName: "red"; colorValue: "(255, 0, 0)"; }
                ListElement { colorName: "rosybrown"; colorValue: "(188, 143, 143)"; }
                ListElement { colorName: "royalblue"; colorValue: "( 65, 105, 225)"; }
                ListElement { colorName: "saddlebrown"; colorValue: "(139, 69, 19)"; }
                ListElement { colorName: "salmon"; colorValue: "(250, 128, 114)"; }
                ListElement { colorName: "sandybrown"; colorValue: "(244, 164, 96)"; }
                ListElement { colorName: "seagreen"; colorValue: "( 46, 139, 87)"; }
                ListElement { colorName: "seashell"; colorValue: "(255, 245, 238)"; }
                ListElement { colorName: "sienna"; colorValue: "(160, 82, 45)"; }
                ListElement { colorName: "silver"; colorValue: "(192, 192, 192)"; }
                ListElement { colorName: "skyblue"; colorValue: "(135, 206, 235)"; }
                ListElement { colorName: "slateblue"; colorValue: "(106, 90, 205)"; }
                ListElement { colorName: "slategray"; colorValue: "(112, 128, 144)"; }
                ListElement { colorName: "slategrey"; colorValue: "(112, 128, 144)"; }
                ListElement { colorName: "snow"; colorValue: "(255, 250, 250)"; }
                ListElement { colorName: "springgreen"; colorValue: "( 0, 255, 127)"; }
                ListElement { colorName: "steelblue"; colorValue: "( 70, 130, 180)"; }
                ListElement { colorName: "tan"; colorValue: "(210, 180, 140)"; }
                ListElement { colorName: "teal"; colorValue: "( 0, 128, 128)"; }
                ListElement { colorName: "thistle"; colorValue: "(216, 191, 216)"; }
                ListElement { colorName: "tomato"; colorValue: "(255, 99, 71)"; }
                ListElement { colorName: "turquoise"; colorValue: "( 64, 224, 208)"; }
                ListElement { colorName: "violet"; colorValue: "(238, 130, 238)"; }
                ListElement { colorName: "wheat"; colorValue: "(245, 222, 179)"; }
                ListElement { colorName: "white"; colorValue: "(255, 255, 255)"; }
                ListElement { colorName: "whitesmoke"; colorValue: "(245, 245, 245)"; }
                ListElement { colorName: "yellow"; colorValue: "(255, 255, 0)"; }
                ListElement { colorName: "yellowgreen"; colorValue: "(154, 205, 50)"; }
            }
            
            Component {
                id: colorListDelegate;
                Rectangle {
                    width: rootRect.width;
                    height: 100;
                    //color: context.model.colorName;
                    color: colorName;
                    Text {
                        anchors.fill: parent;
                        horizontalAlignment: Text.AlignHCenter;
                        verticalAlignment: Text.AlignVCenter;
                        color: "#FF808080";
                        style: Text.Raised;
                        styleColor: "white";
                        //text: context.model.index;
                        text: index;
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("clicked on item: " + index);
                        }
                    }
                }
            }

            ListView {
                id: listView;
                property int myContentY: 0;
                property int myFirstItemIndex: 0;
                
                clip: true;
                interactive: true;
                visible: true;
            
                anchors.fill: parent;
                model: colorListModel;
                delegate: colorListDelegate;
                
                onContentYChanged:
                {
                    //console.log("@@@@ listView: onContentYChanged: contentY: " + contentY);
                    myContentY = contentY;
                }

                onMyContentYChanged:
                {
                    //console.log("@@@@ listView: onMyContentYChanged: myContentY: " + myContentY);
                    myFirstItemIndex = myContentY / 100;
                }

                onMyFirstItemIndexChanged:
                {
                    //console.log("@@@@ listView: onMyFirstItemIndexChanged: myFirstItemIndex: " + myFirstItemIndex);
                }

                onCurrentIndexChanged:
                {
                    //console.log("@@@@ listView: onCurrentIndexChanged: currentIndex: " + currentIndex);
                }

                onCountChanged:
                {
                    //console.log("@@@@ listView: onCountChanged: count: " + count);
                }
            }
                
            Rectangle {
                x: 0;
                y: 620;
                width: parent.width;
                height: 100;

                color: "yellow";

                Text {
                    id: textN;
                    x: 0;
                    y: 0;
                    width: 200;
                    height: 100;

                    color: "red";
                    text: "";
                }

                Rectangle {
                    x: 250;
                    y: 0;
                    width: 100;
                    height: parent.height;

                    color: "gray";

                    Text {
                        anchors.fill: parent;
                        color: "red";
                        text: "Go";
                    }

                    MouseArea {
                        anchors.fill: parent

                        property int currentN: 100;
                        onClicked: {
                            currentN += 43;
                            var n = currentN % 131;
                            textN.text = n;

                            console.log("@@@@ ");

                            console.log("@@@@ pre1: CMD: to show the item: n=" + n);

                            console.log("@@@@ pre1: listView.count=" + listView.count);
                            console.log("@@@@ pre1: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ pre1: listView.contentY: " + listView.contentY);
                            console.log("@@@@ pre1: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));
                            
                            console.log("@@@@ pre1: show the item: n=" + n);
                            listView.positionViewAtIndex(n, ListView.Beginning);
                            
                            console.log("@@@@ post1: listView.count=" + listView.count);
                            console.log("@@@@ post1: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ post1: listView.contentY: " + listView.contentY);
                            console.log("@@@@ post1: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));

                            console.log("@@@@ ");
                        }
                    }
                }

                Rectangle {
                    x: 400;
                    y: 0;
                    width: 100;
                    height: parent.height;

                    color: "gray";

                    Text {
                        anchors.fill: parent;
                        color: "red";
                        text: "Delete";
                    }

                    MouseArea {
                        anchors.fill: parent

                        property int currentN: 100;
                        onClicked: {
                            console.log("@@@@ ");

                            console.log("@@@@ pre2: CMD: to delete items from 20st to the end");

                            console.log("@@@@ pre2: listView.count=" + listView.count);
                            console.log("@@@@ pre2: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ pre2: listView.contentY: " + listView.contentY);
                            console.log("@@@@ pre2: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));

                            if (colorListModel.count > 20) {
                                console.log("@@@@ pre2: remove items from 20rd to the end");
                                colorListModel.remove(20, colorListModel.count - 20);
                            } else {
                                console.log("@@@@ pre2: remove items from 20rd to the end: not items needed to delete");
                            }

                            console.log("@@@@ post2: listView.count=" + listView.count);
                            console.log("@@@@ post2: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ post2: listView.contentY: " + listView.contentY);
                            console.log("@@@@ post2: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));

                            console.log("@@@@ ");
                        }
                    }
                }

                Rectangle {
                    x: 550;
                    y: 0;
                    width: 100;
                    height: parent.height;

                    color: "gray";

                    Text {
                        anchors.fill: parent;
                        color: "red";
                        text: "Go To 2nd";
                    }

                    MouseArea {
                        anchors.fill: parent;

                        property int currentN: 100;

                        onClicked: {
                            currentN = 2;
                            var n = currentN;
                            textN.text = n;

                            console.log("@@@@ ");

                            console.log("@@@@ pre3: CMD: to show the item: n=" + n);

                            console.log("@@@@ pre3: listView.count=" + listView.count);
                            console.log("@@@@ pre3: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ pre3: listView.contentY: " + listView.contentY);
                            console.log("@@@@ pre3: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));

                            console.log("@@@@ pre3: show the item: n=" + n);
                            listView.positionViewAtIndex(n, ListView.Beginning);

                            console.log("@@@@ post3: listView.count=" + listView.count);
                            console.log("@@@@ post3: listView.contentHeight=" + listView.contentHeight);
                            console.log("@@@@ post3: listView.contentY: " + listView.contentY);
                            console.log("@@@@ post3: listView.indexAt(listView.contentX,listView.contentY): " + listView.indexAt(listView.contentX, listView.contentY));

                            console.log("@@@@ ");
                        }
                    }
                }
            }
        }

        
    }
}
